#pragma once
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"
#include "../viewModels/LaunchSettingsViewModel.h"

class IFontRepository;
class ILanguagePackService;
class LanguagePack;
class MaterialColorScheme;

/// @brief Bottom sheet for choosing the loader of a single game.
class LaunchSettingsBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(LaunchSettingsBottomSheetView)
public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(_loaderFieldLabel);
    }

protected:
    void Close() override;

private:
    SharedPtr<LaunchSettingsViewModel> _viewModel;
    const LanguagePack* _languagePack;

    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<Label2DView> _loaderLabel;
    SharedPtr<Label2DView> _loaderFieldLabel;

    const MaterialColorScheme* _materialColorScheme;

    SharedPtr<IconButton2DView> _loaderLeftArrow;
    SharedPtr<IconButton2DView> _loaderRightArrow;

    LaunchSettingsBottomSheetView(SharedPtr<LaunchSettingsViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
        const ILanguagePackService* languagePackService);

    SharedPtr<IconButton2DView> CreateArrowIcon(IconButtonView::button_action_t action);
    void UpdateLabels();
    void UpdateLoaderText();
    u32 LoadIcon(IVramManager& vramManager, const unsigned int* tiles, u32 tilesLength) const;
};
