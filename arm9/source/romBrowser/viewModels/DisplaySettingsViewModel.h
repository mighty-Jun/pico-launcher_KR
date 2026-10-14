#pragma once
#include "../IRomBrowserController.h"
#include "services/settings/RomBrowserDisplaySettings.h"
#include "services/Language/LanguageRepository.h"

/// @brief View model for the display settings screen.
class DisplaySettingsViewModel
{
public:
    explicit DisplaySettingsViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController)
        , _romBrowserDisplaySettings(_romBrowserController->GetRomBrowserDisplaySettings()) { }

    constexpr RomBrowserLayout GetRomBrowserDisplayMode() const
    {
        return _romBrowserDisplaySettings.layout;
    }

    void SetRomBrowserDisplayMode(RomBrowserLayout romBrowserDisplayMode)
    {
        if (_romBrowserDisplaySettings.layout != romBrowserDisplayMode)
        {
            _romBrowserDisplaySettings.layout = romBrowserDisplayMode;
            _romBrowserController->SetRomBrowserDisplaySettings(_romBrowserDisplaySettings);
        }
    }

    constexpr RomBrowserSortMode GetRomBrowserSortMode() const
    {
        return _romBrowserDisplaySettings.sortMode;
    }

    void SetRomBrowserSortMode(RomBrowserSortMode romBrowserSortMode)
    {
        if (_romBrowserDisplaySettings.sortMode != romBrowserSortMode)
        {
            _romBrowserDisplaySettings.sortMode = romBrowserSortMode;
            _romBrowserController->SetRomBrowserDisplaySettings(_romBrowserDisplaySettings);
        }
    }

    void Close()
    {
        _romBrowserController->HideDisplaySettings();
    }

    const LanguageRepository& GetLanguageRepository() const
    {
        return _romBrowserController->GetLanguageRepository();
    }

    const char* GetLanguage() const
    {
        return _romBrowserController->GetLanguage();
    }

    /// @brief Stores the language; the launcher restarts after the settings are closed.
    void SetLanguage(const char* language)
    {
        _romBrowserController->SetLanguage(language);
    }

    void GotoSettingsScreen()
    {
        _romBrowserController->GotoSettingsScreen();
    }

private:
    IRomBrowserController* _romBrowserController;
    RomBrowserDisplaySettings _romBrowserDisplaySettings;
};
