#include "common.h"
#include <nds/bios.h>
#include <libtwl/rtos/rtosIrq.h>
#include <libtwl/rtos/rtosThread.h>
#include <libtwl/rtos/rtosEvent.h>
#include <libtwl/sio/sio.h>
#include <libtwl/spi/spiPmic.h>
#include <libtwl/sys/sysPower.h>
#include <libtwl/sys/swi.h>
#include "lidSleep.h"

// See: https://github.com/blocksds/libnds/blob/master/source/arm7/system.c (PM_REQ_SLEEP)

// Same as the libnds default
#define LID_CLOSED_FRAMES_BEFORE_SLEEP  20

// Below the ipc service threads (5), so sleep only starts while no dldi or sd request is
// being handled, and above the idle thread (0).
#define LID_SLEEP_THREAD_PRIORITY       1

static rtos_thread_t sThread;
static u32 sThreadStack[128];
static rtos_event_t sEvent;
static u32 sLidClosedFrames;

static bool isLidClosed()
{
    return REG_RCNT0_H & RCNT0_H_DATA_LID;
}

static void sleepUntilLidOpened()
{
    u32 irqs = rtos_disableIrqs();
    // the lid may have been opened again while the services were busy
    if (isLidClosed())
    {
        bool soundEnabled = REG_POWCNT & POWCNT_SOUND_ENABLE;
        if (soundEnabled)
            swiChangeSoundBias(false, 0x400);

        // turn off the backlights and the amplifier, and let the power led blink
        u8 pmicControl = pmic_readRegister(PMIC_REG_CONTROL);
        pmic_writeRegister(PMIC_REG_CONTROL, PMIC_CONTROL_POWER_LED_BLINK_SLOW);

        // only wake up when the lid is opened
        u32 irqMask = rtos_getIrqMask();
        u32 irq2Mask = rtos_getIrq2Mask();
        rtos_setIrq2Mask(0);
        rtos_ackIrqMask(RTOS_IRQ_PMIC);
        rtos_setIrqMask(RTOS_IRQ_PMIC);

        swiSleep();
        swi_waitByLoop(8378 * 100); // 100ms

        rtos_ackIrqMask(RTOS_IRQ_PMIC);
        rtos_setIrqMask(irqMask);
        rtos_setIrq2Mask(irq2Mask);

        pmic_writeRegister(PMIC_REG_CONTROL, pmicControl);

        if (soundEnabled)
            swiChangeSoundBias(true, 0x400);
    }
    rtos_restoreIrqs(irqs);
}

static void threadMain(void* arg)
{
    while (true)
    {
        rtos_waitEvent(&sEvent, false, true);
        sleepUntilLidOpened();
    }
}

void lidSleep_init()
{
    rtos_createEvent(&sEvent);
    rtos_createThread(&sThread, LID_SLEEP_THREAD_PRIORITY, threadMain, nullptr,
        sThreadStack, sizeof(sThreadStack));
    rtos_wakeupThread(&sThread);
}

void lidSleep_update()
{
    if (!isLidClosed())
    {
        sLidClosedFrames = 0;
    }
    else if (++sLidClosedFrames == LID_CLOSED_FRAMES_BEFORE_SLEEP)
    {
        // when the lid is still closed after waking up, sleep again after another delay
        sLidClosedFrames = 0;
        rtos_signalEvent(&sEvent);
    }
}
