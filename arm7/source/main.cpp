#include "common.h"
#include <nds/system.h>
#include <libtwl/sound/sound.h>
#include <libtwl/sound/soundChannel.h>
#include <libtwl/sound/soundCapture.h>
#include <libtwl/rtos/rtosIrq.h>
#include <libtwl/rtos/rtosThread.h>
#include <libtwl/rtos/rtosEvent.h>
#include <libtwl/timer/timer.h>
#include <libtwl/sound/sound.h>
#include <libtwl/ipc/ipcSync.h>
#include <libtwl/ipc/ipcFifoSystem.h>
#include <libtwl/sys/sysPower.h>
#include <libtwl/sio/sioRtc.h>
#include <libtwl/sio/sio.h>
#include <libtwl/gfx/gfxStatus.h>
#include <libtwl/mem/memSwap.h>
#include <libtwl/i2c/i2cMcu.h>
#include <libtwl/spi/spiPmic.h>
#include "logger/PlainLogger.h"
#include "logger/NocashOutputStream.h"
#include "logger/NullLogger.h"
#include "logger/ThreadSafeLogger.h"
#include "picoLoaderBootstrap.h"
#include "sharedMemory.h"
#include "ipcServices/DsiSdIpcService.h"
#include "ipcServices/DldiIpcService.h"
#include "ipcServices/SoundIpcService.h"
#include "ipcServices/RtcIpcService.h"
#include "ExitMode.h"
#include "Arm7State.h"
#include "mmc/tmio.h"
#include "touchScreen.h"
#include <nds/arm7/i2c.h>

static NocashOutputStream sNocashOutputStream;
static PlainLogger sPlainLogger = PlainLogger(LogLevel::All, std::unique_ptr<IOutputStream>(&sNocashOutputStream));
static ThreadSafeLogger sThreadSafeLogger = ThreadSafeLogger(std::unique_ptr<ILogger>(&sPlainLogger));

static DsiSdIpcService sDsiSdIpcService;
static DldiIpcService sDldiIpcService;
static SoundIpcService sSoundIpcService;
static RtcIpcService sRtcIpcService;

ILogger* gLogger = &sThreadSafeLogger;

static rtos_event_t sVCountEvent;
static ExitMode sExitMode;
static Arm7State sState;
static volatile u8 sMcuIrqFlag = false;

static void vcountIrq(u32 irqMask)
{
    rtos_signalEvent(&sVCountEvent);
}

static void mcuIrq(u32 irq2Mask)
{
    sMcuIrqFlag = true;
}

static void checkMcuIrq(void)
{
    // mcu only exists in DSi mode
    if (isDSiMode())
    {
        // check and ack the flag atomically
        if (mem_swapByte(false, &sMcuIrqFlag))
        {
            // check the irq mask
            u32 irqMask = mcu_getIrqMask();
            if (irqMask & MCU_IRQ_RESET)
            {
                // power button was released
                sExitMode = ExitMode::Reset;
                sState = Arm7State::ExitRequested;
            }
            else if (irqMask & MCU_IRQ_POWER_OFF)
            {
                // power button was held long to trigger a power off
                sExitMode = ExitMode::PowerOff;
                sState = Arm7State::ExitRequested;
            }
        }
    }
}

static void initializeVCountIrq()
{
    rtos_createEvent(&sVCountEvent);
    gfx_setVCountMatchLine(96);
    rtos_setIrqFunc(RTOS_IRQ_VCOUNT, vcountIrq);
    rtos_enableIrqMask(RTOS_IRQ_VCOUNT);
    gfx_setVCountMatchIrqEnabled(true);
}

static void clearSoundRegisters()
{
    REG_SOUNDCNT = 0;
    REG_SNDCAP0CNT = 0;
    REG_SNDCAP1CNT = 0;

    for (int i = 0; i < 16; i++)
    {
        REG_SOUNDxCNT(i) = 0;
        REG_SOUNDxSAD(i) = 0;
        REG_SOUNDxTMR(i) = 0;
        REG_SOUNDxPNT(i) = 0;
        REG_SOUNDxLEN(i) = 0;
    }
}

static void initializeArm7()
{
    rtos_initIrq();
    rtos_startMainThread();
    ipc_initFifoSystem();

    clearSoundRegisters();

    SHARED_IS_3DS_FLAG = 0; // 기본값은 0 (DSi 또는 구형 DS)
    if (isDSiMode()) 
    {
        u8 byteBak = i2cReadRegister(I2C_PM, 0x71); 
        i2cWriteRegister(I2C_PM, 0x71, 0xD2);
        u8 byteNew = i2cReadRegister(I2C_PM, 0x71);
        
        // 0xD2 쓰기가 막혔다면 3DS(TWL_FIRM) 환경이므로 1을 기록합니다.
        if (byteNew != 0xD2) 
        {
            SHARED_IS_3DS_FLAG = 1;
        }
        
        i2cWriteRegister(I2C_PM, 0x71, byteBak);
    }

    pmic_setAmplifierEnable(true);
    sys_setSoundPower(true);

    readUserSettings();
    pmic_setPowerLedBlink(PMIC_CONTROL_POWER_LED_BLINK_NONE);

    sio_setGpioSiIrq(false);
    sio_setGpioMode(RCNT0_L_MODE_GPIO);

    rtc_init();

    if (isDSiMode())
    {
        TMIO_init();
        sDsiSdIpcService.Start();
    }

    sDldiIpcService.Start();
    pload_init();

    snd_setMasterVolume(127);
    snd_setMasterEnable(true);
    sSoundIpcService.Start();
    sRtcIpcService.Start();

    initializeVCountIrq();

    if (isDSiMode())
    {
        rtos_setIrq2Func(RTOS_IRQ2_MCU, mcuIrq);
        rtos_enableIrq2Mask(RTOS_IRQ2_MCU);
    }

    touch_init();

    ipc_setArm7SyncBits(7);
}

static void updateArm7IdleState()
{
    if (pload_shouldStart())
    {
        sExitMode = ExitMode::PicoLoader;
        sState = Arm7State::ExitRequested;
    }
    else
    {
        checkMcuIrq();
    }

    if (sState == Arm7State::ExitRequested)
    {
        snd_setMasterVolume(0); // mute sound
    }
    static int batteryTimer = 0;
    if (++batteryTimer >= 60)
    {
        SHARED_BATTERY_LEVEL = getBatteryLevel();
        batteryTimer = 0;
    }
}

static bool performExit(ExitMode exitMode)
{
    switch (exitMode)
    {
        case ExitMode::Reset:
        {
            mcu_setWarmBootFlag(true);
            mcu_hardReset();
            break;
        }
        case ExitMode::PowerOff:
        {
            pmic_shutdown();
            break;
        }
        case ExitMode::PicoLoader:
        {
            pload_start();
            break;
        }
    }

    while (true); // wait infinitely for exit
}

static void updateArm7ExitRequestedState()
{
    performExit(sExitMode);
}

static void updateArm7()
{
    switch (sState)
    {
        case Arm7State::Idle:
        {
            updateArm7IdleState();
            break;
        }
        case Arm7State::ExitRequested:
        {
            updateArm7ExitRequestedState();
            break;
        }
    }
}

int main()
{
    sState = Arm7State::Idle;
    initializeArm7();

    while (true)
    {
        rtos_waitEvent(&sVCountEvent, true, true);
        u16 keys = REG_RCNT0_H | RCNT0_H_DATA_PEN;
        touchPosition touchPos;
        if (touch_update(touchPos))
        {
            keys &= ~RCNT0_H_DATA_PEN; // pen down
            SHARED_TOUCH_X = touchPos.px;
            SHARED_TOUCH_Y = touchPos.py;
        }
        SHARED_KEY_XY = keys;
        updateArm7();
    }

    return 0;
}
