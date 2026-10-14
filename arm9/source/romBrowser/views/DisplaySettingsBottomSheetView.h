#pragma once
#include <array>
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"
#include "SelectorFieldView.h"
#include "../viewModels/DisplaySettingsViewModel.h"

class IRomBrowserController;
class MaterialColorScheme;
class IFontRepository;
class ILanguagePackService;

class DisplaySettingsBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(DisplaySettingsBottomSheetView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    void SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken,
        const SelectorFieldView::VramToken& selectorFieldVramToken);

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(_layoutOptions[0]);
    }

protected:
    void Close() override;

private:
    DisplaySettingsViewModel* _viewModel;
    const ILanguagePackService* _languagePackService;

    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<Label2DView> _layoutLabel;
    SharedPtr<Label2DView> _sortingLabel;
    // LabelView _filtersLabel;
    SharedPtr<SelectorFieldView> _languageField;

    std::array<SharedPtr<IconButton2DView>, 4> _layoutOptions;
    std::array<SharedPtr<IconButton2DView>, /*3*/2> _sortOptions;
    // std::array<IconButton2DView, 5> _filterOptions;

    const MaterialColorScheme* _materialColorScheme;

    /// @brief The language shown in the language field; only stored when A is pressed.
    String<char, 32> _selectedLanguage;

    SharedPtr<IconButton2DView> CreateLayoutOptionIconButton();
    SharedPtr<IconButton2DView> CreateSortOptionIconButton();
    // IconButton2DView CreateFilterOptionIconButton();

    DisplaySettingsBottomSheetView(DisplaySettingsViewModel* viewModel, 
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository, 
        const ILanguagePackService* languagePackService);

    void UpdateLabels();
    void SelectLanguage(int direction);

    u32 LoadIcon(IVramManager& vramManager, const unsigned int* tiles, u32 tilesLength) const;
};
