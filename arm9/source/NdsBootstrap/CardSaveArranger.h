#pragma once
/// @brief Class for setting up the save file for retail card roms.
class CardSaveArranger
{
public:
    bool SetupCardSave(const TCHAR* romPath, const TCHAR* savePath) const;
};