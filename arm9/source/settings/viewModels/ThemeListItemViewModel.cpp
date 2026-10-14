#include "common.h"
#include "settings/ISettingsController.h"
#include "ThemeListItemViewModel.h"

void ThemeListItemViewModel::Activate()
{
    if (_extraThemeInfo)
    {
        _settingsController->SelectTheme(_extraThemeInfo->themeInfo->GetFolderName());
    }
}
