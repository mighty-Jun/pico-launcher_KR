#pragma once
#include <optional>
#include "../IRomBrowserController.h"

/// @brief View model for the per-game launch settings screen.
class LaunchSettingsViewModel
{
public:
    explicit LaunchSettingsViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController)
        , _loader(romBrowserController->GetGameLoader()) { }

    /// @brief Returns the selected loader, or \c std::nullopt when the default loader is selected.
    std::optional<LoaderType> GetLoader() const { return _loader; }

    void SelectNextLoader() { SelectLoader(1); }
    void SelectPreviousLoader() { SelectLoader(-1); }

    /// @brief Stores the selected loader for the game and closes the screen.
    void Apply()
    {
        _romBrowserController->SetGameLoader(_loader);
        Close();
    }

    /// @brief Closes the screen without storing the selection.
    void Close()
    {
        _romBrowserController->HideLaunchSettings();
    }

private:
    IRomBrowserController* _romBrowserController;
    std::optional<LoaderType> _loader;

    void SelectLoader(int offset)
    {
        static constexpr std::optional<LoaderType> sLoaders[] =
        {
            std::nullopt,
            LoaderType::PicoLoader,
            LoaderType::NdsBootstrap
        };
        constexpr int count = sizeof(sLoaders) / sizeof(sLoaders[0]);

        int idx = 0;
        while (idx < count && sLoaders[idx] != _loader)
            idx++;
        _loader = sLoaders[(idx + offset + count) % count];
    }
};
