#include "common.h"
#include <strings.h>
#include "GameSettings.h"

std::optional<LoaderType> GameSettings::GetLoader(const char* romPath) const
{
    const auto entry = FindEntry(romPath);
    if (!entry)
        return std::nullopt;
    return entry->loader;
}

void GameSettings::SetLoader(const char* romPath, std::optional<LoaderType> loader)
{
    auto entry = FindEntry(romPath);
    if (!loader)
    {
        if (entry)
        {
            // keep the array compact by moving the last entry into the freed slot
            *entry = _entries[--_entryCount];
        }
    }
    else if (entry)
    {
        entry->loader = *loader;
    }
    else
    {
        if (_entryCount == _entryCapacity)
        {
            u32 newCapacity = _entryCapacity == 0 ? 8 : _entryCapacity * 2;
            auto newEntries = std::make_unique_for_overwrite<Entry[]>(newCapacity);
            for (u32 i = 0; i < _entryCount; i++)
                newEntries[i] = _entries[i];
            _entries = std::move(newEntries);
            _entryCapacity = newCapacity;
        }
        _entries[_entryCount].romPath = romPath;
        _entries[_entryCount].loader = *loader;
        _entryCount++;
    }
}

GameSettings::Entry* GameSettings::FindEntry(const char* romPath)
{
    return const_cast<Entry*>(static_cast<const GameSettings*>(this)->FindEntry(romPath));
}

const GameSettings::Entry* GameSettings::FindEntry(const char* romPath) const
{
    for (u32 i = 0; i < _entryCount; i++)
    {
        // FAT paths are case-insensitive
        if (!strcasecmp(_entries[i].romPath.GetString(), romPath))
            return &_entries[i];
    }
    return nullptr;
}
