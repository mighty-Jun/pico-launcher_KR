#pragma once
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"
#include "SelectorFieldView.h"
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
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;

    void SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken,
        const SelectorFieldView::VramToken& selectorFieldVramToken);

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(_loaderField->GetFocusView());
    }

protected:
    void Close() override;

private:
    SharedPtr<LaunchSettingsViewModel> _viewModel;
    const LanguagePack* _languagePack;
    const MaterialColorScheme* _materialColorScheme;

    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<SelectorFieldView> _loaderField;

    LaunchSettingsBottomSheetView(SharedPtr<LaunchSettingsViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
        const ILanguagePackService* languagePackService);

    void UpdateLoaderText();
};
