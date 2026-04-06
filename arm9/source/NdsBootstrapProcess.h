#pragma once
#include "common.h"
#include "cheats/GameCheats.h"
#include "fat/ff.h"
#include "services/settings/IAppSettingsService.h"

/// @brief nds-bootstrap을 구동하기 위한 INI 파일을 생성하고 체인로딩을 수행하는 헬퍼 클래스입니다.
class NdsBootstrapProcess
{
public:
    /// @brief 선택된 롬과 세이브 정보를 바탕으로 nds-bootstrap.ini를 작성하고 실행합니다.
    static void Launch();

    static bool PrepareCheats(const GameCheats* cheats);

private:
    static bool PrepareIni(const char* romPath, const char* savePath, bool isDsiRom);
    static bool HasValidDsiBinary(const char* romPath);
    static u32 WriteActiveCheats(FIL* file, const CheatEntry* entry);
    static int GetLanguageCode(const char* langStr);
};