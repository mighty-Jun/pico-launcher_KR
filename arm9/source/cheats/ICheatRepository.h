#pragma once
#include "GameCheats.h"
#include "fat/FastFileRef.h"

/// @brief Interface for a repository providing access to cheats.
class ICheatRepository
{
public:
    virtual ~ICheatRepository() { }

    /// @brief Gets the available cheats for the given \p romFile.
    /// @param romFile Reference to the rom file.
    /// @return A unique pointer to the found cheats, or an empty unique pointer when no cheats were found.
    virtual std::unique_ptr<GameCheats> GetCheatsForGame(const FastFileRef& romFile) const = 0;

    /// @brief Writes the enable/disabled status of the given \p cheats.
    /// @param cheats The cheats to update.
    virtual void UpdateEnabledCheatsForGame(const std::unique_ptr<GameCheats>& cheats) const = 0;

protected:
    ICheatRepository() { }
};
