#pragma once
#include <memory>
#include "UsrCheatRepository.h"

/// @brief Factory for constructing a \see UsrCheatRepository for a usrcheat.dat file.
class UsrCheatRepositoryFactory
{
public:
    /// @brief Constructs a \see UsrCheatRepository for the given \p usrCheatDatPath.
    /// @param usrCheatDatPath The path to the usrcheat.dat file.
    /// @return A unique pointer to the constructed repository, or an empty unique pointer when construction failed.
    std::unique_ptr<UsrCheatRepository> FromUsrCheatDat(const TCHAR* usrCheatDatPath);
};
