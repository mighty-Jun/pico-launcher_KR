#pragma once
#include <memory>
#include <optional>
#include "core/String.h"
#include "LoaderType.h"

/// @brief Settings that apply to a single game, identified by its full rom path.
class GameSettings
{
public:
    struct Entry
    {
        String<char, 256> romPath;
        LoaderType loader;
    };

    /// @brief Returns the loader chosen for the rom at \p romPath.
    /// @return The loader, or \c std::nullopt when the default loader should be used.
    std::optional<LoaderType> GetLoader(const char* romPath) const;

    /// @brief Sets the loader for the rom at \p romPath. \c std::nullopt removes the
    ///        override so the default loader is used again.
    void SetLoader(const char* romPath, std::optional<LoaderType> loader);

    const Entry* GetEntries() const { return _entries.get(); }
    u32 GetEntryCount() const { return _entryCount; }

private:
    // std::vector is avoided on purpose: it pulls in libstdc++ exception support,
    // whose static constructor allocates before the heap is initialized.
    std::unique_ptr<Entry[]> _entries;
    u32 _entryCount = 0;
    u32 _entryCapacity = 0;

    Entry* FindEntry(const char* romPath);
    const Entry* FindEntry(const char* romPath) const;
};
