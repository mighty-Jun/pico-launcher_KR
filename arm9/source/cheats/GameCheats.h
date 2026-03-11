#pragma once
#include "Cheat.h"
#include "CheatCategory.h"
#include "ICheatCategory.h"

/// @brief Class holding the cheats for a game.
class GameCheats : public ICheatCategory
{
public:
    GameCheats(std::unique_ptr<u8[]> cheatData, u32 cheatDataLength, u32 fileOffset, const char* gameName,
        CheatCategory* categories, u32 numberOfCategories, Cheat* cheats, u32 numberOfCheats)
        : _cheatData(std::move(cheatData)), _cheatDataLength(cheatDataLength), _fileOffset(fileOffset), _gameName(gameName)
        , _categories(categories), _numberOfCategories(numberOfCategories)
        , _cheats(cheats), _numberOfCheats(numberOfCheats) { }

    ~GameCheats()
    {
        if (_categories != nullptr)
        {
            free(_categories);
        }
        if (_cheats != nullptr)
        {
            free(_cheats);
        }
    }

    /// @brief Gets the name of the game.
    /// @return A pointer to the name of the game.
    const char* GetGameName() const
    {
        return _gameName;
    }

    const CheatCategory* GetCategories(u32& numberOfCategories) const override
    {
        numberOfCategories = _numberOfCategories;
        return _categories;
    }

    const Cheat* GetCheats(u32& numberOfCheats) const override
    {
        numberOfCheats = _numberOfCheats;
        return _cheats;
    }

    /// @brief Gets a pointer to the cheat data.
    /// @param cheatDataLength The length of the cheat data is returned in this reference.
    /// @return A pointer to the cheat data. This pointer is only valid for the lifetime of this \see GameCheats instance.
    u8* GetCheatData(u32& cheatDataLength) const
    {
        cheatDataLength = _cheatDataLength;
        return _cheatData.get();
    }

    u32 GetFileOffset() const
    {
        return _fileOffset;
    }

private:
    std::unique_ptr<u8[]> _cheatData;
    u32 _cheatDataLength;
    u32 _fileOffset;
    const char* _gameName;
    CheatCategory* _categories;
    u32 _numberOfCategories;
    Cheat* _cheats;
    u32 _numberOfCheats;

    // From ICheatCategory
    const char* GetName() const override
    {
        return "";
    }

    bool GetIsMaxOneCheatActive() const override
    {
        return false;
    }
};
