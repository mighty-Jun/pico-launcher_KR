#pragma once

class CheatCategory;
class Cheat;

/// @brief Interface for a collection of cheats.
class ICheatCategory
{
public:
    virtual ~ICheatCategory() = default;

    /// @brief Gets the name of this category.
    /// @return The name of this category.
    virtual const char* GetName() const = 0;

    /// @brief Indicates if only one cheat in this category is allowed to be on or not.
    /// @return \c true when only one cheat is allowed to be active in this category, or \c false otherwise.
    virtual bool GetIsMaxOneCheatActive() const = 0;

    /// @brief Gets the sub-categories of this category.
    /// @param numberOfCategories The number of categories is returned through this reference.
    /// @return A pointer to an array of categories. This may be \c nullptr when \p numberOfCategories is 0.
    virtual const CheatCategory* GetCategories(u32& numberOfCategories) const;

    /// @brief Gets the cheats in this category.
    /// @param numberOfCheats The number of cheats is returned through this reference.
    /// @return A pointer to an array of cheats. This may be \c nullptr when \p numberOfCheats is 0.
    virtual const Cheat* GetCheats(u32& numberOfCheats) const;

protected:
    ICheatCategory() = default;
};
