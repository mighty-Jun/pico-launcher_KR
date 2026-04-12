#include "common.h"
#include <algorithm>
#include "SaveList.h"

const SaveListEntry* SaveList::FindEntry(u32 gameCode)
{
    if (_count != 0)
    {
        const auto gameEntry = std::lower_bound(_entries.get(), _entries.get() + _count, gameCode,
            [] (const SaveListEntry& entry, u32 value)
            {
                return entry.GetGameCode() < value;
            });

        if (gameEntry != _entries.get() + _count && gameEntry->GetGameCode() == gameCode)
        {
            return gameEntry;
        }
    }

    return nullptr;
}
