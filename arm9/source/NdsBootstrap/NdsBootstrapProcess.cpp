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
#include "sharedMemory.h"
#include "CardSaveArranger.h"
//#include "NdsInternalFileInfo.h"

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

    f_lseek(&file, 0x012);
    f_read(&file, &unitCode, 1, &bytesRead);

    if (unitCode == 0x00)
    {
        f_close(&file);
        return false;
    }

    u32 arm9iOffset = 0;
    u32 arm7iOffset = 0;

    f_lseek(&file, 0x1C0);
    f_read(&file, &arm9iOffset, 4, &bytesRead);

    f_lseek(&file, 0x1C8);
    f_read(&file, &arm7iOffset, 4, &bytesRead);

    if (arm9iOffset < 0x8000 || arm9iOffset >= 0x20000000 ||
        arm7iOffset < 0x8000 || arm7iOffset >= 0x20000000)
    {
        f_close(&file);
        LOG_DEBUG("Stripped DSi ROM detected (Invalid Offsets).\n");
        return false;
    }

    u32 arm9Sig[3][4] = {0};

    f_lseek(&file, 0x8000);
    f_read(&file, arm9Sig[0], sizeof(u32) * 4, &bytesRead);

    f_lseek(&file, arm9iOffset);
    f_read(&file, arm9Sig[1], sizeof(u32) * 4, &bytesRead);

    f_lseek(&file, arm7iOffset);
    f_read(&file, arm9Sig[2], sizeof(u32) * 4, &bytesRead);

    f_close(&file);

    for (int i = 1; i < 3; i++)
    {
        if (arm9Sig[i][0] == arm9Sig[0][0] &&
            arm9Sig[i][1] == arm9Sig[0][1] &&
            arm9Sig[i][2] == arm9Sig[0][2] &&
            arm9Sig[i][3] == arm9Sig[0][3])
        {
            LOG_DEBUG("Stripped DSi ROM detected (Cloned ARM9 Signature).\n");
            return false;
        }

        if (arm9Sig[i][0] == 0 &&
            arm9Sig[i][1] == 0 &&
            arm9Sig[i][2] == 0 &&
            arm9Sig[i][3] == 0)
        {
            LOG_DEBUG("Stripped DSi ROM detected (Zeroed Signature).\n");
            return false;
        }

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

    // 🔥 unaligned 경고와 스택 오버플로우를 동시에 잡는 4바이트 정렬 static 선언
    alignas(4) static FIL iniFile; 
    FRESULT result = f_open(&iniFile, "fat:/_nds/nds-bootstrap.ini", FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        LOG_ERROR("Failed to create nds-bootstrap.ini (Error: %d)\n", result);
        return false;
    }

    // 🔥 힙 고갈(Data Abort)의 주범이었던 new char[512]를 static 정렬 배열로 교체!
    alignas(4) static char buffer[512];

    bool enableDsiMode = Environment::IsDsiMode() && isDsiRom;
    const char* consoleModel = "0";
    if (Environment::IsDsiMode())
    {
        // 현재 구동하시는 기기가 뉴큰다수(New 3DS XL)이므로, 
        // SHARED_IS_3DS_FLAG가 1로 정상 인식되어 consoleModel이 '2'로 세팅될 것입니다.
        consoleModel = (SHARED_IS_3DS_FLAG == 1) ? "2" : "0"; 
    }

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
        consoleModel
    );

    UINT bytesWritten;
    result = f_write(&iniFile, buffer, len, &bytesWritten);
    f_close(&iniFile);

    if (result != FR_OK || bytesWritten != (UINT)len)
    {
        LOG_ERROR("Failed to write data to nds-bootstrap.ini\n");
        return false;
    }

    LOG_DEBUG("Successfully generated nds-bootstrap.ini\n");
    return true;
}

u32 NdsBootstrapProcess::WriteActiveCheats(FIL* file, const CheatEntry* entry)
{
    if (entry == nullptr) return 0;
    
    u32 totalWritten = 0;

    if (entry->IsCheatCategory())
    {
        u32 subCount = 0;
        const CheatEntry* subEntries = entry->GetSubEntries(subCount);
        if (subEntries != nullptr)
        {
            for (u32 i = 0; i < subCount; i++)
            {
                totalWritten += WriteActiveCheats(file, &subEntries[i]);
            }
        }
    }
    else
    {
        if (entry->GetIsCheatActive())
        {
            u32 len = 0;
            const void* data = entry->GetCheatData(len);
            
            if (data != nullptr && len > 0)
            {
                UINT bytesWritten;
                f_write(file, data, len, &bytesWritten);
                totalWritten += bytesWritten;
            }
        }
    }
    
    return totalWritten;
}

bool NdsBootstrapProcess::PrepareCheats(const GameCheats* cheats)
{
    const char* cheatPath = "fat:/_nds/nds-bootstrap/cheatData.bin";

    f_unlink(cheatPath);

    if (cheats == nullptr)
    {
        return true;
    }

    f_mkdir("fat:/_nds/nds-bootstrap");

    FIL file;
    if (f_open(&file, cheatPath, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        LOG_ERROR("Failed to create cheatData.bin\n");
        return false;
    }

    u32 totalBytes = 0;
    u32 subCount = 0;
    const CheatEntry* subEntries = cheats->GetSubEntries(subCount);
    
    if (subEntries != nullptr)
    {
        for (u32 i = 0; i < subCount; i++)
        {
            totalBytes += WriteActiveCheats(&file, &subEntries[i]);
        }
    }

    if (totalBytes > 0)
    {
        // EOF inject
        u32 terminator[1] = { 0xCF000000 };
        UINT bytesWritten;
        f_write(&file, terminator, 4, &bytesWritten);
        f_close(&file);
        
        LOG_DEBUG("Successfully generated cheatData.bin with CF000000 Terminator.\n");
    }
    else
    {
        f_close(&file);
        f_unlink(cheatPath);
        LOG_DEBUG("No active cheats found. cheatData.bin removed.\n");
    }

    return true;
}

void NdsBootstrapProcess::Launch(const NdsInternalFileInfo* internalInfo)
{
    auto loadParams = pload_getLoadParams();

    static char targetRom[256];
    static char targetSave[256];
    
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

    CardSaveArranger saveArranger;
    LOG_DEBUG("Calling CardSaveArranger to setup save file...\n");
    if (!saveArranger.SetupCardSave(targetRom, targetSave))
    {
        LOG_ERROR("Failed to process save file through CardSaveArranger.\n");
        // 필요 시 return; 
    }

    bool isValidDsi = HasValidDsiBinary(targetRom);
    bool iniResult = PrepareIni(targetRom, targetSave, isValidDsi);

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

int NdsBootstrapProcess::GetLanguageCode(const char* langStr)
{
    if (langStr == nullptr) return -1;
    
    if (strcmp(langStr, "korean") == 0) return 7;
    if (strcmp(langStr, "english") == 0) return 1;
    if (strcmp(langStr, "japanese") == 0) return 0;

    return -1; // default
}