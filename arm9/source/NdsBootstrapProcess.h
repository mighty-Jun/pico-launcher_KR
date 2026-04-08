#pragma once
#include "common.h"
#include "cheats/GameCheats.h"
#include "fat/ff.h"
#include "services/settings/IAppSettingsService.h"

class NdsBootstrapProcess
{
public:
    static void Launch();

    static bool PrepareCheats(const GameCheats* cheats);

private:
    static bool PrepareIni(const char* romPath, const char* savePath, bool isDsiRom);
    static bool HasValidDsiBinary(const char* romPath);
    static u32 WriteActiveCheats(FIL* file, const CheatEntry* entry);
    static int GetLanguageCode(const char* langStr);
};