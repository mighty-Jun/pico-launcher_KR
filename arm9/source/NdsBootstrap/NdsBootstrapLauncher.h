#pragma once
#include "common.h"
#include "cheats/GameCheats.h"
#include "fat/ff.h"

/// @brief Launches nds roms through nds-bootstrap (fat:/_nds/nds-bootstrap-release.nds).
class NdsBootstrapLauncher
{
public:
    /// @brief Prepares the save file, cheat data and nds-bootstrap.ini for the rom at
    ///        \p romPath, and switches to the pico loader process to boot nds-bootstrap.
    /// @param romPath The nds rom to launch.
    /// @param cheats The cheats of the rom, or \c nullptr when there are none.
    static void Launch(const char* romPath, const GameCheats* cheats);

private:
    static bool PrepareCheats(const GameCheats* cheats);
    static bool PrepareIni(const char* romPath, const char* savePath, bool isDsiRom);
    static bool HasValidDsiBinary(const char* romPath);
    static u32 WriteActiveCheats(FIL* file, const CheatEntry* entry);
};