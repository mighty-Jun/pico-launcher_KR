#include "NdsBootstrapProcess.h"
#include "core/Environment.h"
#include "core/StringUtil.h"
#include "fat/ff.h"
#include "logger/ILogger.h"
#include "services/process/ProcessManager.h"
#include "PicoLoaderProcess.h"
#include <stdio.h>
#include "picoLoaderBootstrap.h"

extern ProcessManager gProcessManager; // 전역 프로세스 매니저 참조

bool NdsBootstrapProcess::PrepareIni(const char* romPath, const char* savePath)
{
    // 1. _nds 폴더 생성 (이미 존재하면 FR_EXIST 에러를 반환하므로 무시해도 무방함)
    f_mkdir("fat:/_nds");

    // 2. nds-bootstrap.ini 파일 생성 및 쓰기 모드로 열기 (기존 파일이 있으면 덮어씀)
    FIL iniFile;
    FRESULT result = f_open(&iniFile, "fat:/_nds/nds-bootstrap.ini", FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        LOG_ERROR("Failed to create nds-bootstrap.ini (Error: %d)\n", result);
        return false;
    }

    // 3. INI 파일 내용 포맷팅 (snprintf 사용)
    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer),
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
        Environment::IsDsiMode() ? "1" : "0",
        Environment::IsDsiMode() ? "2" : "0"
    );

    // 4. 버퍼의 내용을 파일에 쓰기
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

void NdsBootstrapProcess::Launch()
{
    auto loadParams = pload_getLoadParams();

    // 1. 기존 파라미터에서 사용자가 선택한 타겟 게임 롬과 세이브 경로를 백업합니다.
    char targetRom[256];
    char targetSave[256];
    StringUtil::Copy(targetRom, loadParams->romPath, sizeof(targetRom));
    StringUtil::Copy(targetSave, loadParams->savePath, sizeof(targetSave));

    // 2. 백업한 경로를 바탕으로 INI 지시서를 작성합니다.
    if (!PrepareIni(targetRom, targetSave))
    {
        LOG_ERROR("Aborting launch due to INI generation failure.\n");
        // TODO: 실패 시 다이얼로그나 에러 메시지를 띄우는 로직을 추가할 수 있습니다.
        return;
    }

    // 3. 피코 로더에게 nds-bootstrap 자체를 롬으로써 실행하도록 파라미터를 '변조'합니다.
    StringUtil::Copy(loadParams->romPath, "fat:/_nds/nds-bootstrap-release.nds", sizeof(loadParams->romPath));
    
    // nds-bootstrap이 INI를 보고 알아서 세이브와 치트를 마운트하므로, 
    // 피코 런처 단의 세이브 마운트 및 인자 전달은 끕니다.
    loadParams->savePath[0] = 0; 
    loadParams->arguments[0] = 0;
    loadParams->argumentsLength = 0;
    pload_setCheatData(nullptr);

    // 4. PicoLoaderProcess로 상태를 전환하여 체인로딩(부팅)을 시작합니다!
    gProcessManager.Goto<PicoLoaderProcess>();
}