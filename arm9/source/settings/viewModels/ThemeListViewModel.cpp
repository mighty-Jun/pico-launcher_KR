#include "common.h"
#include "settings/ISettingsController.h"
#include "ThemeListViewModel.h"

ThemeListViewModel::ThemeListViewModel(ISettingsController* settingsController)
    : _settingsController(settingsController) { }

void ThemeListViewModel::NavigateUp() const
{
    _settingsController->NavigateUp();
}
