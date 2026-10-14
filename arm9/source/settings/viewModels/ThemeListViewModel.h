#pragma once

class ISettingsController;

class ThemeListViewModel
{
public:
    explicit ThemeListViewModel(ISettingsController* settingsController);

    void NavigateUp() const;

    ISettingsController* GetSettingsController() const
    {
        return _settingsController;
    }

    constexpr int GetSelectedItem() const { return _selectedItem; }
    void SetSelectedItem(int selectedItem) { _selectedItem = selectedItem; }

private:
    ISettingsController* _settingsController;
    int _selectedItem = -1;
};
