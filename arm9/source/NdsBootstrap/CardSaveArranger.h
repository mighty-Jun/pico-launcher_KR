#pragma once
/// @brief Class for setting up the save file for retail card roms.
class CardSaveArranger
{
public:
    /// @brief 롬 파일 경로를 통해 헤더를 읽고 세이브 파일을 설정합니다.
    /// @param romPath 롬 파일(.nds)의 절대 경로
    /// @param savePath 생성할 세이브 파일(.sav)의 절대 경로
    bool SetupCardSave(const TCHAR* romPath, const TCHAR* savePath) const;
};