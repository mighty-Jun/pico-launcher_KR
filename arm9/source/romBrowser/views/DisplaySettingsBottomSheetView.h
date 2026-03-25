#pragma once
#include <array>
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"
#include "../viewModels/DisplaySettingsViewModel.h"

class IRomBrowserController;
class MaterialColorScheme;
class IFontRepository;
class IAppSettingsService;
class ILanguagePackService;


class DisplaySettingsBottomSheetView : public BottomSheetView
{
public:
    DisplaySettingsBottomSheetView(DisplaySettingsViewModel* viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
        IAppSettingsService* appSettingsService, ILanguagePackService* languagePackService);

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    View* MoveFocus(View* currentFocus,
        FocusMoveDirection direction, View* source) override;

    void SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken);

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(&_layoutOptions[0]);
    }

private:
    DisplaySettingsViewModel* _viewModel;
    IAppSettingsService* _appSettingsService;
    ILanguagePackService* _languagePackService;

    Label2DView _titleLabel;
    Label2DView _layoutLabel;
    Label2DView _sortingLabel;
// LabelView _filtersLabel;
    Label2DView _themeLabel;
    Label2DView _themeFieldLabel;
    
    Label2DView _languageLabel;
    Label2DView _languageFieldLabel;

    std::array<IconButton2DView, 4> _layoutOptions;
    std::array<IconButton2DView, /*3*/2> _sortOptions;
    // std::array<IconButton2DView, 5> _filterOptions;

    const MaterialColorScheme* _materialColorScheme;

    String<char, 64> _appliedThemeName;
    String<char, 64> _appliedLanguageName;

    static constexpr int kMaxThemeCount = 16;
    std::array<String<char, 64>, kMaxThemeCount> _themeNames;
    int _themeCount = 0;
    int _selectedThemeIdx = 0;
    bool _themesLoaded = false;
    String<char, 64> _pendingThemeName;

    static constexpr int kMaxLanguageCount = 16;
    struct LanguageEntry
    {
        String<char, 64> fileName;
        char16_t displayName[64];
    };
    std::array<LanguageEntry, kMaxLanguageCount> _languageEntries;
    int _languageCount = 0;
    int _selectedLanguageIdx = 0;
    bool _languagesLoaded = false;
    String<char, 64> _pendingLanguageName;

    IconButton2DView CreateLayoutOptionIconButton();
    IconButton2DView CreateSortOptionIconButton();
    // IconButton2DView CreateFilterOptionIconButton();

    void UpdateLabels();
    //Theme, Language function
    void LoadThemes();
    void EnsureThemesLoaded();
    void UpdateThemeUI();
    void ChangeTheme(int newIdx);
    void LoadLanguages();
    void EnsureLanguagesLoaded();
    void UpdateLanguageUI();
    void ChangeLanguage(int newIdx);
    void ReleaseLazyLists();

    u32 LoadIcon(IVramManager& vramManager, const unsigned int* tiles, u32 tilesLength) const;

    bool _themeLongPressConsumed = false;
    bool _usePreloadedIcons = false;
};
