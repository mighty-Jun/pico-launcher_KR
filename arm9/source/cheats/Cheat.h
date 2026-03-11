#pragma once

/// @brief Class representing a single cheat.
class Cheat
{
public:
    Cheat() { }

    Cheat(const char* name, const char* description, u32* flagsPointer, const void* cheatData, u32 cheatDataLength)
        : _name(name), _description(description), _flagsPointer(flagsPointer)
        , _cheatData(cheatData), _cheatDataLength(cheatDataLength) { }

    /// @brief Gets the name of this cheat.
    /// @return A pointer to the name of this cheat.
    const char* GetName() const
    {
        return _name;
    }

    /// @brief Gets the description of this cheat.
    /// @return A pointer to the description of this cheat.
    const char* GetDescription() const
    {
        return _description;
    }

    /// @brief Gets a pointer to the data of this cheat.
    /// @param cheatDataLength The length of the cheat data is returned in this reference.
    /// @return A pointer to the cheat data.
    ///         This pointer is only valid for the lifetime of the \see GameCheats instance this cheat belongs to.
    const void* GetCheatData(u32& cheatDataLength) const
    {
        cheatDataLength = _cheatDataLength;
        return _cheatData;
    }

    /// @brief Gets whether this cheat is active (enabled) or not.
    /// @return \c true when this cheat is active, or \c false when not active.
    bool GetIsCheatActive() const
    {
        return ((*_flagsPointer >> 24) & 1) == 1;
    }

    /// @brief Sets whether this cheat is active (enabled) or not.
    /// @param isCheatActive \c true to enable this cheat, or \c false to disable this cheat.
    void SetIsCheatActive(bool isCheatActive) const
    {
        u32 flags = *_flagsPointer;
        flags &= ~(1 << 24);
        if (isCheatActive)
        {
            flags |= 1 << 24;
        }
        *_flagsPointer = flags;
    }

private:
    const char* _name = nullptr;
    const char* _description = nullptr;
    u32* _flagsPointer = nullptr;
    const void* _cheatData = nullptr;
    u32 _cheatDataLength = 0;
};
