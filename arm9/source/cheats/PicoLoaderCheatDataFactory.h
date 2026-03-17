#pragma once
#include <memory>
#include "GameCheats.h"
#include "picoLoader7.h"

/// @brief Factory for creating Pico Loader compatible cheat data.
class PicoLoaderCheatDataFactory
{
public:
    /// @brief Converts the given \p gameCheats to Pico Loader format. Only the enabled cheats will be included.
    /// @param gameCheats The cheats to convert.
    /// @return Pointer to the created cheat data.
    pload_cheats_t* CreateCheatData(const std::unique_ptr<GameCheats>& gameCheats) const;

private:
    u32 GetCheatEntryRequiredSize(const CheatEntry* cheatEntry, u32& totalNumberOfCheats) const;
    void GetCheatEntryData(const CheatEntry* cheatEntry, u8*& buffer) const;
};
