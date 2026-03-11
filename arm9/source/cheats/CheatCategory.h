#pragma once
#include <memory>
#include "ICheatCategory.h"

/// @brief Class representing a cheat category, containing sub-categories and cheats.
class CheatCategory : public ICheatCategory
{
public:
    CheatCategory() { }

    CheatCategory(const char* name, const char* description, bool isMaxOneCheatActive,
        CheatCategory* subCategories, u32 numberOfSubCategories, Cheat* cheats, u32 numberOfCheats)
        : _name(name), _description(description), _isMaxOneCheatActive(isMaxOneCheatActive)
        , _subCategories(subCategories), _numberOfSubCategories(numberOfSubCategories)
        , _cheats(cheats), _numberOfCheats(numberOfCheats) { }

    CheatCategory(CheatCategory& other) = delete;
    CheatCategory& operator=(CheatCategory& other) = delete;

    CheatCategory(CheatCategory&& other)
    {
        *this = std::move(other);
    }

    CheatCategory& operator=(CheatCategory&& other)
    {
        _name = other._name;
        other._name = nullptr;
        _description = other._description;
        other._description = nullptr;
        _isMaxOneCheatActive = other._isMaxOneCheatActive;
        other._isMaxOneCheatActive = false;
        _subCategories = other._subCategories;
        other._subCategories = nullptr;
        _numberOfSubCategories = other._numberOfSubCategories;
        other._numberOfSubCategories = 0;
        _cheats = other._cheats;
        other._cheats = nullptr;
        _numberOfCheats = other._numberOfCheats;
        other._numberOfCheats = 0;
        return *this;
    }

    ~CheatCategory()
    {
        if (_subCategories != nullptr)
        {
            free(_subCategories);
        }
        if (_cheats != nullptr)
        {
            free(_cheats);
        }
    }

    const char* GetName() const override
    {
        return _name;
    }

    /// @brief Gets the description of this cheat category.
    /// @return A pointer to the description of this cheat category.
    const char* GetDescription() const
    {
        return _description;
    }

    bool GetIsMaxOneCheatActive() const override
    {
        return _isMaxOneCheatActive;
    }

    const CheatCategory* GetCategories(u32& numberOfCategories) const override
    {
        numberOfCategories = _numberOfSubCategories;
        return _subCategories;
    }

    const Cheat* GetCheats(u32& numberOfCheats) const override
    {
        numberOfCheats = _numberOfCheats;
        return _cheats;
    }

private:
    const char* _name = nullptr;
    const char* _description = nullptr;
    bool _isMaxOneCheatActive = false;
    CheatCategory* _subCategories = nullptr;
    u32 _numberOfSubCategories = 0;
    Cheat* _cheats = nullptr;
    u32 _numberOfCheats = 0;
};
