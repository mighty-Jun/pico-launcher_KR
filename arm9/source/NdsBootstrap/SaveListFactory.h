#pragma once
#include <memory>
#include "SaveList.h"

/// @brief Factory for creating \see SaveList instances.
class SaveListFactory
{
public:
    /// @brief Creates a \see SaveList instance from the file at the given \p path.
    /// @param path The save list file path.
    /// @return A pointer to the constructed \see SaveList instance, or \c nullptr if construction failed.
    std::unique_ptr<SaveList> CreateFromFile(const TCHAR* path);
};
