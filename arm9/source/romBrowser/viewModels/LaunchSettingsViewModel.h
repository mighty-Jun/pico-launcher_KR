#pragma once
#include "../IRomBrowserController.h"
#include "services/settings/RomBrowserDisplaySettings.h"

/// @brief View model for the display settings screen.
class LaunchSettingsViewModel
{
public:
    explicit LaunchSettingsViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController) { }

    
    void Close()
    {
        _romBrowserController->HideLaunchSettings();
    }

    void HideLaunchSettings()
    {
        _romBrowserController->HideLaunchSettings();
    }

    void SaveSettingsNow()
    {
        _romBrowserController->SaveSettingsNow();
    }


private:
    IRomBrowserController* _romBrowserController;
};
