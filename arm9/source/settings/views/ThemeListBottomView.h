#pragma once
#include <memory>
#include "ThemeAdapter.h"
#include "gui/views/RecyclerView.h"
#include "gui/views/ViewContainer.h"
#include "settings/viewModels/ThemeListViewModel.h"
#include "settings/views/SettingsAppBarView.h"

class IFontRepository;
class ISettingsController;
class MaterialColorScheme;

class ThemeListBottomView : public ViewContainer
{
    SHARED_ONLY(ThemeListBottomView)

public:
    ThemeListBottomView(SharedPtr<ThemeListViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
        const IRomBrowserViewFactory* romBrowserViewFactory, const IThemeFileIconFactory* themeFileIconFactory,
        VBlankTextureLoader* vblankTextureLoader);

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus, FocusMoveDirection direction, View* source) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position, 256, 192);
    }

    void Focus(FocusManager& focusManager)
    {
        _recyclerView->Focus(focusManager);
    }

    SharedPtr<SettingsAppBarView> _appBarView;
private:
    SharedPtr<RecyclerView> _recyclerView;
    SharedPtr<ThemeAdapter> _themeAdapter;
    SharedPtr<ThemeListViewModel> _viewModel;
};
