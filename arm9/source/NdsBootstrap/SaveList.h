#pragma once
#include <memory>
#include "common.h"
#include "CardSaveType.h"

/// @brief Class representing a save list entry.
class SaveListEntry
{
    u32 gameCode;
    CardSaveType saveType;
    u8 saveSize; // 0 or 1 << x
    u8 reserved[2]; // for possible future use

public:
    u32 GetGameCode() const { return gameCode; }
    CardSaveType GetSaveType() const { return saveType; }
    u32 GetSaveSize() const { return saveSize == 0 ? 0 : (1u << saveSize); }

    void Dump() const
    {
        const char* saveType;
        switch (GetSaveType())
        {
            case CardSaveType::None:
            {
                saveType = "none";
                break;
            }
            case CardSaveType::Eeprom:
            {
                saveType = "eeprom";
                break;
            }
            case CardSaveType::Flash:
            {
                saveType = "flash";
                break;
            }
            case CardSaveType::Nand:
            {
                saveType = "nand";
                break;
            }
            default:
            {
                saveType = "unknown";
                break;
            }
        }
        LOG_DEBUG("%c%c%c%c - %s - 0x%X\n",
            gameCode & 0xFF, (gameCode >> 8) & 0xFF, (gameCode >> 16) & 0xFF, gameCode >> 24,
            saveType,
            GetSaveSize());
    }
};

static_assert(sizeof(SaveListEntry) == 8, "Invalid sizeof(SaveListEntry)");

/// @brief Class representing a save list.
class SaveList
{
public:
    SaveList(std::unique_ptr<const SaveListEntry[]> entries, u32 count)
        : _entries(std::move(entries)), _count(count) { }

    /// @brief Attempts to find the save list entry for the given \p gameCode.
    /// @param gameCode The game code to search for.
    /// @return A pointer to the \see SaveListEntry when found, or \c nullptr otherwise.
    const SaveListEntry* FindEntry(u32 gameCode);

private:
    std::unique_ptr<const SaveListEntry[]> _entries;
    u32 _count;
};
