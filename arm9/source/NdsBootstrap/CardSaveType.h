#pragma once

/// @brief Enum representing the save type.
enum class CardSaveType : u8
{
    /// @brief The game has no save.
    None = 0,

    /// @brief EEPROM save.
    Eeprom = 1,

    /// @brief Flash save.
    Flash = 2,

    /// @brief NAND save.
    Nand = 3
};
