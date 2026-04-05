#include "NdsBootstrapProcess.h"
#include "core/Environment.h"
#include "core/StringUtil.h"
#include "core/mini-printf.h"
#include "fat/ff.h"
#include "logger/ILogger.h"
#include "services/process/ProcessManager.h"
#include "PicoLoaderProcess.h"
#include <string.h>
#include "picoLoaderBootstrap.h"
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>

#define FIFO_PICO_MSG_IS_3DS 0x1234

extern ProcessManager gProcessManager;

bool NdsBootstrapProcess::HasValidDsiBinary(const char* romPath)
{
    FIL file;
    if (f_open(&file, romPath, FA_READ) != FR_OK)
    {
        return false;
    }

    UINT bytesRead;
    u8 unitCode = 0;

    // 1. Unit Code 검사 (오프셋 0x012)
    f_lseek(&file, 0x012);
    f_read(&file, &unitCode, 1, &bytesRead);

    // 일반 DS 롬이면 켤 필요가 없으므로 즉시 false 반환
    if (unitCode == 0x00)
    {
        f_close(&file);
        return false;
    }

    // 2. DSi 바이너리 오프셋 읽기 (0x1C0: ARM9i Offset, 0x1C8: ARM7i Offset)
    u32 arm9iOffset = 0;
    u32 arm7iOffset = 0;

    f_lseek(&file, 0x1C0);
    f_read(&file, &arm9iOffset, 4, &bytesRead);

    f_lseek(&file, 0x1C8);
    f_read(&file, &arm7iOffset, 4, &bytesRead);

    // 3. 오프셋 범위 유효성 검사 (TWLMenu 로직: 0x8000 미만이거나 512MB 이상인 경우)
    if (arm9iOffset < 0x8000 || arm9iOffset >= 0x20000000 ||
        arm7iOffset < 0x8000 || arm7iOffset >= 0x20000000)
    {
        f_close(&file);
        LOG_DEBUG("Stripped DSi ROM detected (Invalid Offsets).\n");
        return false;
    }

    // 4. 시그니처 검사를 위한 배열 (4바이트씩 4개 = 16바이트)
    u32 arm9Sig[3][4] = {0};

    // 기준이 되는 일반 ARM9 시그니처 (0x8000)
    f_lseek(&file, 0x8000);
    f_read(&file, arm9Sig[0], sizeof(u32) * 4, &bytesRead);

    // ARM9i 시그니처
    f_lseek(&file, arm9iOffset);
    f_read(&file, arm9Sig[1], sizeof(u32) * 4, &bytesRead);

    // ARM7i 시그니처
    f_lseek(&file, arm7iOffset);
    f_read(&file, arm9Sig[2], sizeof(u32) * 4, &bytesRead);

    f_close(&file);

    // 5. 시그니처 비교 분석 (TWLMenu 로직)
    for (int i = 1; i < 3; i++)
    {
        // 트릭 1: 일반 ARM9 데이터를 복사해서 돌려막기 한 경우
        if (arm9Sig[i][0] == arm9Sig[0][0] &&
            arm9Sig[i][1] == arm9Sig[0][1] &&
            arm9Sig[i][2] == arm9Sig[0][2] &&
            arm9Sig[i][3] == arm9Sig[0][3])
        {
            LOG_DEBUG("Stripped DSi ROM detected (Cloned ARM9 Signature).\n");
            return false;
        }

        // 트릭 2: 0x00으로 데이터를 날려버린 경우
        if (arm9Sig[i][0] == 0 &&
            arm9Sig[i][1] == 0 &&
            arm9Sig[i][2] == 0 &&
            arm9Sig[i][3] == 0)
        {
            LOG_DEBUG("Stripped DSi ROM detected (Zeroed Signature).\n");
            return false;
        }

        // 트릭 3: 0xFF로 데이터를 날려버린 경우
        if (arm9Sig[i][0] == 0xFFFFFFFF &&
            arm9Sig[i][1] == 0xFFFFFFFF &&
            arm9Sig[i][2] == 0xFFFFFFFF &&
            arm9Sig[i][3] == 0xFFFFFFFF)
        {
            LOG_DEBUG("Stripped DSi ROM detected (0xFF Signature).\n");
            return false;
        }
    }

    LOG_DEBUG("Valid DSi binary detected.\n");
    return true;
}

bool NdsBootstrapProcess::PrepareIni(const char* romPath, const char* savePath, bool isDsiRom)
{
    f_mkdir("fat:/_nds");

    FIL iniFile;
    FRESULT result = f_open(&iniFile, "fat:/_nds/nds-bootstrap.ini", FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        LOG_ERROR("Failed to create nds-bootstrap.ini (Error: %d)\n", result);
        return false;
    }

    // ★ 해결책: 스택 오버플로우 방지를 위해 배열을 힙(Heap) 메모리에 할당합니다.
    char* buffer = new char[512];

    bool enableDsiMode = Environment::IsDsiMode() && isDsiRom;

    int len = mini_snprintf(buffer, 512,
        "[NDS-BOOTSTRAP]\n"
        "NDS_PATH = %s\n"
        "SAV_PATH = %s\n"
        "DSI_MODE = %s\n"
        "CONSOLE_MODEL = %s\n"
        "LANGUAGE = -1\n"
        "REGION = -1\n"
        "LOGGING = 1\n"
        "DEBUG = 1\n",
        romPath,
        (savePath != nullptr) ? savePath : "",
        enableDsiMode ? "1" : "0",
        Environment::IsDsiMode() ? "2" : "0"
    );

    UINT bytesWritten;
    result = f_write(&iniFile, buffer, len, &bytesWritten);
    f_close(&iniFile);
    
    // 사용이 끝난 힙 메모리는 누수(Leak)가 없도록 즉시 해제합니다.
    delete[] buffer;

    if (result != FR_OK || bytesWritten != (UINT)len)
    {
        LOG_ERROR("Failed to write data to nds-bootstrap.ini\n");
        return false;
    }

    LOG_DEBUG("Successfully generated nds-bootstrap.ini\n");
    return true;
}

void NdsBootstrapProcess::Launch()
{
    auto loadParams = pload_getLoadParams();

    // ★ 해결책: 경로를 담을 256바이트 배열들도 힙(Heap)에 할당합니다.
    char* targetRom = new char[256];
    char* targetSave = new char[256];
    
    StringUtil::Copy(targetRom, loadParams->romPath, 256);

    if (loadParams->savePath[0] != '\0')
    {
        StringUtil::Copy(targetSave, loadParams->savePath, 256);
    }
    else
    {
        StringUtil::Copy(targetSave, targetRom, 256);
        char* ext = strrchr(targetSave, '.');
        if (ext != nullptr)
        {
            strcpy(ext, ".sav");
        }
        else
        {
            strcat(targetSave, ".sav");
        }
    }

    bool isValidDsi = HasValidDsiBinary(targetRom);
    bool iniResult = PrepareIni(targetRom, targetSave, isValidDsi);
    
    // INI 작성이 끝났으므로 힙 메모리를 해제합니다.
    delete[] targetRom;
    delete[] targetSave;

    if (!iniResult)
    {
        LOG_ERROR("Aborting launch due to INI generation failure.\n");
        return;
    }

    StringUtil::Copy(loadParams->romPath, "fat:/_nds/nds-bootstrap-release.nds", sizeof(loadParams->romPath));
    
    loadParams->savePath[0] = 0; 
    loadParams->arguments[0] = 0;
    loadParams->argumentsLength = 0;
    pload_setCheatData(nullptr);

    gProcessManager.Goto<PicoLoaderProcess>();
}