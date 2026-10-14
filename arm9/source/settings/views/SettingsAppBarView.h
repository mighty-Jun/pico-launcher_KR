#pragma once
#include "romBrowser/views/AppBarView.h"
#include "gui/views/ViewContainer.h"
#include "settings/viewModels/ThemeListViewModel.h"

class RomBrowserDisplayMode;
class IRomBrowserViewFactory;

class SettingsAppBarView : public ViewContainer
{
    SHARED_ONLY(SettingsAppBarView)

public:
    void InitVram(const VramContext& vramContext) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus, FocusMoveDirection direction, View* source) override;

    void Focus(FocusManager& focusManager)
    {
        _appBarView->Focus(focusManager, 0);
    }

private:
    enum AppBarButton
    {
        APP_BAR_BUTTON_BACK = 0
    };

    SharedPtr<ThemeListViewModel> _viewModel;
    SharedPtr<AppBarView> _appBarView;

    SettingsAppBarView(SharedPtr<ThemeListViewModel> viewModel, const IRomBrowserViewFactory* romBrowserViewFactory);
};
