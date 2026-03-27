#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/VramContext.h"
#include "gui/IVramManager.h"
#include "hGridIcon.h"
#include "vGridIcon.h"
#include "bannerListIcon.h"
#include "listIcon.h"
#include "sortNameAscendingIcon.h"
#include "sortNameDescendingIcon.h"
#include "recentIcon.h"
#include "gamesIcon.h"
#include "picturesIcon.h"
#include "musicIcon.h"
#include "moviesIcon.h"
#include "unknownIcon.h"
#include "coverflowIcon.h"
#include "../IRomBrowserController.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "DisplaySettingsBottomSheetView.h"
#include <services/settings/IAppSettingsService.h>
#include <services/Language/ILanguagePackService.h>
#include "fat/Directory.h"
#include "left_icon.h"
#include "right_icon.h"

#define TITLE_LABEL_X       20
#define TITLE_LABEL_Y       16

#define LAYOUT_LABEL_X      20
#define LAYOUT_LABEL_Y      46

#define SORTING_LABEL_X     20
#define SORTING_LABEL_Y     78

#define FILTERS_LABEL_X     20
#define FILTERS_LABEL_Y     112

#define THEME_LABEL_X       20
#define THEME_LABEL_Y       107

#define LANGUAGE_LABEL_X    20
#define LANGUAGE_LABEL_Y    131 // +24

#define THEME_FIELD_X       100
#define LANGUAGE_FIELD_X    100

static RomBrowserLayout sRomBrowserDisplayModes[4] =
{
    [0] = RomBrowserLayout::HorizontalIconGrid,
    [1] = RomBrowserLayout::VerticalIconGrid,
    [2] = RomBrowserLayout::BannerList,
    [3] = RomBrowserLayout::CoverFlow
};

static RomBrowserSortMode sRomBrowserSortModes[4] =
{
    [0] = RomBrowserSortMode::NameAscending,
    [1] = RomBrowserSortMode::NameDescending,
    [2] = RomBrowserSortMode::LastModified
};

DisplaySettingsBottomSheetView::DisplaySettingsBottomSheetView(
    DisplaySettingsViewModel* viewModel, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository, IAppSettingsService* appSettingsService,
    ILanguagePackService* languagePackService)
    : _viewModel(viewModel)
    , _appSettingsService(appSettingsService)
    , _languagePackService(languagePackService)
    , _titleLabel(128, 16, 25, fontRepository->GetFont(FontType::Medium11))
    , _layoutLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
    , _sortingLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
    , _themeLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
    , _themeFieldLabel(120, 16, 20, fontRepository->GetFont(FontType::Regular10))
    , _languageLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
    , _languageFieldLabel(120, 16, 20, fontRepository->GetFont(FontType::Regular10))
    , _materialColorScheme(materialColorScheme)
    // , _filtersLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
{
    const auto& langPack = _languagePackService->GetLanguagePack();
    //langPack = languagePackService->GetLanguagePack();
    //load langPack string instead of hard coding
    _titleLabel.SetText(langPack.displaySettings_title.GetString());
    _layoutLabel.SetText(langPack.displaySettings_layout.GetString());
    _sortingLabel.SetText(langPack.displaySettings_sorting.GetString());
    _themeLabel.SetText(langPack.displaySettings_theme.GetString());
    _languageLabel.SetText(langPack.displaySettings_langugage.GetString());
    
    const char* currTheme = _appSettingsService->GetAppSettings().theme.GetString();
    _pendingThemeName = (currTheme && currTheme[0] != 0) ? currTheme : "material";
    _appliedThemeName = _pendingThemeName;
    
    const char* currLang = _appSettingsService->GetAppSettings().language.GetString();
    _pendingLanguageName = (currLang && currLang[0] != 0) ? currLang : "english";
    _appliedLanguageName = _pendingLanguageName;

    _themeFieldLabel.SetText(_pendingThemeName.GetString());
    _languageFieldLabel.SetText(_pendingLanguageName.GetString());

    AddChildTail(&_titleLabel);
    AddChildTail(&_layoutLabel);
    AddChildTail(&_sortingLabel);
    AddChildTail(&_themeLabel);
    AddChildTail(&_themeFieldLabel);
    AddChildTail(&_languageLabel);
    AddChildTail(&_languageFieldLabel);

    _themeLeftArrow = CreateArrowIcon();
    _themeRightArrow = CreateArrowIcon();
    _langLeftArrow = CreateArrowIcon();
    _langRightArrow = CreateArrowIcon();

    AddChildTail(&_themeLeftArrow);
    AddChildTail(&_themeRightArrow);
    AddChildTail(&_langLeftArrow);
    AddChildTail(&_langRightArrow);

    for (auto& layoutOption : _layoutOptions)
    {
        layoutOption = CreateLayoutOptionIconButton();
        AddChildTail(&layoutOption);
    }

    for (auto& sortOption : _sortOptions)
    {
        sortOption = CreateSortOptionIconButton();
        AddChildTail(&sortOption);
    }

    // for (auto& filterOption : _filterOptions)
    // {
    //     filterOption = CreateFilterOptionIconButton();
    //     AddChildTail(&filterOption);
    // }

    // _filterOptions[0].SetState(IconButtonView::State::ToggleSelected);
}

IconButton2DView DisplaySettingsBottomSheetView::CreateLayoutOptionIconButton()
{
    IconButton2DView layoutOption
    {
        IconButtonView::Type::Tonal,
        IconButtonView::State::ToggleUnselected,
        md::sys::color::surfaceContainerLow,
        _materialColorScheme
    };
    layoutOption.SetAction([] (IconButtonView* sender, void* arg)
    {
        auto self = reinterpret_cast<DisplaySettingsBottomSheetView*>(arg);
        u32 idx = ((IconButton2DView*)sender) - &self->_layoutOptions[0];
        self->_viewModel->SetRomBrowserDisplayMode(sRomBrowserDisplayModes[idx]);
    }, this);
    return layoutOption;
}

IconButton2DView DisplaySettingsBottomSheetView::CreateSortOptionIconButton()
{
    IconButton2DView sortOption
    {
        IconButtonView::Type::Tonal,
        IconButtonView::State::ToggleUnselected,
        md::sys::color::surfaceContainerLow,
        _materialColorScheme
    };
    sortOption.SetAction([] (IconButtonView* sender, void* arg)
    {
        auto self = reinterpret_cast<DisplaySettingsBottomSheetView*>(arg);
        u32 idx = ((IconButton2DView*)sender) - &self->_sortOptions[0];
        self->_viewModel->SetRomBrowserSortMode(sRomBrowserSortModes[idx]);
    }, this);
    return sortOption;
}

// IconButtonView DisplaySettingsBottomSheetView::CreateFilterOptionIconButton()
// {
//     IconButtonView filterOption
//     {
//         IconButtonView::Type::Tonal,
//         IconButtonView::State::ToggleUnselected,
//         md::sys::color::surfaceContainerLow,
//         _materialColorScheme
//     };
//     return filterOption;
// }

void DisplaySettingsBottomSheetView::InitVram(const VramContext& vramContext)
{
    BottomSheetView::InitVram(vramContext);

    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        // layout options
        _layoutOptions[0].SetIconVramOffset(LoadIcon(*objVramManager, hGridIconTiles, hGridIconTilesLen));
        _layoutOptions[1].SetIconVramOffset(LoadIcon(*objVramManager, vGridIconTiles, vGridIconTilesLen));
        _layoutOptions[2].SetIconVramOffset(LoadIcon(*objVramManager, bannerListIconTiles, bannerListIconTilesLen));
        _layoutOptions[3].SetIconVramOffset(LoadIcon(*objVramManager, coverflowIconTiles, coverflowIconTilesLen));

        // sort options
        _sortOptions[0].SetIconVramOffset(LoadIcon(*objVramManager, sortNameAscendingIconTiles, sortNameAscendingIconTilesLen));
        _sortOptions[1].SetIconVramOffset(LoadIcon(*objVramManager, sortNameDescendingIconTiles, sortNameDescendingIconTilesLen));
        // _sortOptions[2].SetIconVramOffset(LoadIcon(objVramManager, recentIconTiles, recentIconTilesLen));

        // filter options
        // _filterOptions[0].SetIconVramOffset(LoadIcon(objVramManager, gamesIconTiles, gamesIconTilesLen));
        // _filterOptions[1].SetIconVramOffset(LoadIcon(objVramManager, picturesIconTiles, picturesIconTilesLen));
        // _filterOptions[2].SetIconVramOffset(LoadIcon(objVramManager, musicIconTiles, musicIconTilesLen));
        // _filterOptions[3].SetIconVramOffset(LoadIcon(objVramManager, moviesIconTiles, moviesIconTilesLen));
        // _filterOptions[4].SetIconVramOffset(LoadIcon(objVramManager, unknownIconTiles, unknownIconTilesLen));
        
        // 1. VRAM에 그래픽을 한 번만 올립니다.
        _leftArrowIconVramOffset = LoadIcon(*objVramManager, left_iconTiles, left_iconTilesLen);
        _rightArrowIconVramOffset = LoadIcon(*objVramManager, right_iconTiles, right_iconTilesLen);

        // 2. ★ 수정: 올린 그래픽 주소를 4개의 객체에 각각 연결해 줍니다.
        _themeLeftArrow.SetIconVramOffset(_leftArrowIconVramOffset);
        _themeRightArrow.SetIconVramOffset(_rightArrowIconVramOffset);
        _langLeftArrow.SetIconVramOffset(_leftArrowIconVramOffset);
        _langRightArrow.SetIconVramOffset(_rightArrowIconVramOffset);
    }
}

void DisplaySettingsBottomSheetView::UpdateLabels()
{
    _titleLabel.SetPosition(TITLE_LABEL_X, _position.y + TITLE_LABEL_Y);
    _layoutLabel.SetPosition(LAYOUT_LABEL_X, _position.y + LAYOUT_LABEL_Y);
    _sortingLabel.SetPosition(SORTING_LABEL_X, _position.y + SORTING_LABEL_Y);
    // _filtersLabel.SetPosition(FILTERS_LABEL_X, _position.y + FILTERS_LABEL_Y);
    _themeLabel.SetPosition(THEME_LABEL_X, _position.y + THEME_LABEL_Y);
    _themeFieldLabel.SetPosition(THEME_FIELD_X, _position.y + THEME_LABEL_Y);
    _languageLabel.SetPosition(LANGUAGE_LABEL_X, _position.y + LANGUAGE_LABEL_Y);
    _languageFieldLabel.SetPosition(LANGUAGE_FIELD_X, _position.y + LANGUAGE_LABEL_Y);

    _themeLeftArrow.SetPosition(82, _position.y + THEME_LABEL_Y - 7);
    _themeRightArrow.SetPosition(180, _position.y + THEME_LABEL_Y - 7);
    
    _langLeftArrow.SetPosition(82, _position.y + LANGUAGE_LABEL_Y - 7);
    _langRightArrow.SetPosition(180, _position.y + LANGUAGE_LABEL_Y - 7);
}

void DisplaySettingsBottomSheetView::Update()
{
    BottomSheetView::Update();
    /*if (_themeSettleCounter > 0)
    {
        if (--_themeSettleCounter == 0)
        {
            _appSettingsService->GetAppSettings().theme = _pendingThemeName.GetString();
            _settingsDirty = true;
            SaveIfDirty();
        }
    }
    if (_languageSettleCounter > 0)
    {
        if (--_languageSettleCounter == 0)
            SaveIfDirty();
    }*/
    
    UpdateLabels();
    auto selectedDisplayMode = _viewModel->GetRomBrowserDisplayMode();
    int x = 70;
    u32 idx = 0;
    for (auto& layoutOption : _layoutOptions)
    {
        layoutOption.SetPosition(x, _position.y + 38);
        layoutOption.SetState(sRomBrowserDisplayModes[idx] == selectedDisplayMode
            ? IconButtonView::State::ToggleSelected
            : IconButtonView::State::ToggleUnselected);
        x += 32;
        idx++;
    }
    auto selectedSortMode = _viewModel->GetRomBrowserSortMode();
    x = 70;
    idx = 0;
    for (auto& sortOption : _sortOptions)
    {
        sortOption.SetPosition(x, _position.y + 70);
        sortOption.SetState(sRomBrowserSortModes[idx] == selectedSortMode
            ? IconButtonView::State::ToggleSelected
            : IconButtonView::State::ToggleUnselected);
        x += 32;
        idx++;
    }
    // x = 70;
    // for (auto& filterOption : _filterOptions)
    // {
    //     filterOption.SetPosition(x, _position.y + 102);
    //     x += 32;
    // }
}

void DisplaySettingsBottomSheetView::Draw(GraphicsContext& graphicsContext)
{
    graphicsContext.SetClipArea(GetBounds());
    u32 oldPrio = graphicsContext.SetPriority(1);
    {
        _titleLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _titleLabel.SetForegroundColor(_materialColorScheme->onSurface);
        _layoutLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _layoutLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);
        _sortingLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _sortingLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);
        // _filtersLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        // _filtersLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);
        _themeLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _themeLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);
        // ★ 테마 값 라벨: 포커스 여부에 따라 색상 반전 처리
        bool themeFocused = _themeFieldLabel.IsFocused();
        _themeFieldLabel.SetBackgroundColor(themeFocused
            ? _materialColorScheme->GetColor(md::sys::color::secondaryContainer)
            : _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _themeFieldLabel.SetForegroundColor(themeFocused
            ? _materialColorScheme->GetColor(md::sys::color::primary)
            : _materialColorScheme->onSurfaceVariant);


            
        _languageLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _languageLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);

        // ★ 언어 값 라벨: 포커스 여부에 따라 색상 반전 처리
        bool langFocused = _languageFieldLabel.IsFocused();
        _languageFieldLabel.SetBackgroundColor(langFocused
            ? _materialColorScheme->GetColor(md::sys::color::secondaryContainer)
            : _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _languageFieldLabel.SetForegroundColor(langFocused
            ? _materialColorScheme->GetColor(md::sys::color::primary)
            : _materialColorScheme->onSurfaceVariant);
        BottomSheetView::Draw(graphicsContext);
    }
    graphicsContext.SetPriority(oldPrio);
    graphicsContext.ResetClipArea();
}

bool DisplaySettingsBottomSheetView::HandleInput(
    const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (_themeFieldLabel.IsFocused()) EnsureThemesLoaded();
    if (_languageFieldLabel.IsFocused()) EnsureLanguagesLoaded();

    if ((_themeFieldLabel.IsFocused() || _languageFieldLabel.IsFocused()) && inputProvider.Triggered(InputKey::A))
    {
        _appSettingsService->GetAppSettings().theme = _pendingThemeName.GetString();
        _appSettingsService->GetAppSettings().language = _pendingLanguageName.GetString();
        
        _viewModel->SaveSettingsNow();

        if (strcasecmp(_pendingThemeName.GetString(), _appliedThemeName.GetString()) != 0 ||
            strcasecmp(_pendingLanguageName.GetString(), _appliedLanguageName.GetString()) != 0)
        {
            _viewModel->RequestThemeReload();
        }

        ReleaseLazyLists();
        _viewModel->Close();
        return true;
    }

    if (inputProvider.Triggered(InputKey::B))
    {
        ReleaseLazyLists();

        _viewModel->Close();
        return true;
    }
    return false;
}

View* DisplaySettingsBottomSheetView::MoveFocus(View* currentFocus,
    FocusMoveDirection direction, View* source)
{
    int idx = 0;
    for (auto& layoutOption : _layoutOptions)
    {
        if (currentFocus == &layoutOption)
        {
            if (direction == FocusMoveDirection::Left)
            {
                if (--idx < 0)
                    idx += _layoutOptions.size();
                return &_layoutOptions[idx];
            }
            else if (direction == FocusMoveDirection::Right)
            {
                if (++idx >= (int)_layoutOptions.size())
                    idx = 0;
                return &_layoutOptions[idx];
            }
            // else if (direction == FocusMoveDirection::Up)
            // {
            //     if (idx >= (int)_filterOptions.size())
            //         idx = _filterOptions.size() - 1;
            //     return &_filterOptions[idx];
            // }
            else if (direction == FocusMoveDirection::Up)
            {
                // 최상단이므로 맨 아래 언어 라벨로 순환 이동
                return &_languageFieldLabel;
            }
            else //if (direction == FocusMoveDirection::Down)
            {
                if (idx >= (int)_sortOptions.size())
                    idx = _sortOptions.size() - 1;
                return &_sortOptions[idx];
            }
        }
        idx++;
    }
    idx = 0;
    for (auto& sortOption : _sortOptions)
    {
        if (currentFocus == &sortOption)
        {
            if (direction == FocusMoveDirection::Left)
            {
                if (--idx < 0)
                    idx += _sortOptions.size();
                return &_sortOptions[idx];
            }
            else if (direction == FocusMoveDirection::Right)
            {
                if (++idx >= (int)_sortOptions.size())
                    idx = 0;
                return &_sortOptions[idx];
            }
            else if (direction == FocusMoveDirection::Up)
            {
                if (idx >= (int)_layoutOptions.size()) idx = _layoutOptions.size() - 1;
                return &_layoutOptions[idx];
            }
            else // Down
            {
                // Sort 아래에는 Theme가 있음
                return &_themeFieldLabel;
            }
            //else //if (direction == FocusMoveDirection::Up)
            //{
            //    if (idx >= (int)_layoutOptions.size())
            //        idx = _layoutOptions.size() - 1;
            //    return &_layoutOptions[idx];
            //}
            // else //if (direction == FocusMoveDirection::Down)
            // {
            //     if (idx >= (int)_filterOptions.size())
            //         idx = _filterOptions.size() - 1;
            //     return &_filterOptions[idx];
            // }
        }
        idx++;
    }
    // idx = 0;
    // for (auto& filterOption : _filterOptions)
    // {
    //     if (currentFocus == &filterOption)
    //     {
    //         if (direction == FocusMoveDirection::Left)
    //         {
    //             if (--idx < 0)
    //                 idx += _filterOptions.size();
    //             return &_filterOptions[idx];
    //         }
    //         else if (direction == FocusMoveDirection::Right)
    //         {
    //             if (++idx >= (int)_filterOptions.size())
    //                 idx = 0;
    //             return &_filterOptions[idx];
    //         }
    //         else if (direction == FocusMoveDirection::Up)
    //         {
    //             if (idx >= (int)_sortOptions.size())
    //                 idx = _sortOptions.size() - 1;
    //             return &_sortOptions[idx];
    //         }
    //         else //if (direction == FocusMoveDirection::Down)
    //         {
    //             if (idx >= (int)_layoutOptions.size())
    //                 idx = _layoutOptions.size() - 1;
    //             return &_layoutOptions[idx];
    //         }
    //     }
    //     idx++;
    // }
    // 3. Theme Value Label 탐색
    if (currentFocus == &_themeFieldLabel)
    {
        EnsureThemesLoaded();
        
        if (direction == FocusMoveDirection::Left)
        {
            if (_themeCount > 0) {
                int newIdx = (_selectedThemeIdx - 1 + _themeCount) % _themeCount;
                ChangeTheme(newIdx);
            }
            return &_themeFieldLabel;
        }
        else if (direction == FocusMoveDirection::Right)
        {
            if (_themeCount > 0) {
                int newIdx = (_selectedThemeIdx + 1) % _themeCount;
                ChangeTheme(newIdx);
            }
            return &_themeFieldLabel;
        }
        
        if (direction == FocusMoveDirection::Up)
            return &_sortOptions[0]; // 위로 가면 Sort
        if (direction == FocusMoveDirection::Down)
            return &_languageFieldLabel; // 아래로 가면 Language
    }
    
    // 4. Language Value Label 탐색
    if (currentFocus == &_languageFieldLabel)
    {
        EnsureLanguagesLoaded();
        
        if (direction == FocusMoveDirection::Left)
        {
            if (_languageCount > 0) {
                int newIdx = (_selectedLanguageIdx - 1 + _languageCount) % _languageCount;
                ChangeLanguage(newIdx);
            }
            return &_languageFieldLabel;
        }
        else if (direction == FocusMoveDirection::Right)
        {
            if (_languageCount > 0) {
                int newIdx = (_selectedLanguageIdx + 1) % _languageCount;
                ChangeLanguage(newIdx);
            }
            return &_languageFieldLabel;
        }
        
        if (direction == FocusMoveDirection::Up)
            return &_themeFieldLabel; // 위로 가면 Theme
        if (direction == FocusMoveDirection::Down)
            return &_layoutOptions[0]; // 아래로 가면 다시 최상단 Layout으로 순환
    }
    return nullptr;
}

void DisplaySettingsBottomSheetView::SetGraphics(
    const IconButton2DView::VramToken& iconButtonVramToken)
{
    for (auto& layoutOption : _layoutOptions)
        layoutOption.SetGraphics(iconButtonVramToken);
    for (auto& sortOption : _sortOptions)
        sortOption.SetGraphics(iconButtonVramToken);
    // for (auto& filterOption : _filterOptions)
    //     filterOption.SetGraphics(iconButtonVramToken);
}

u32 DisplaySettingsBottomSheetView::LoadIcon(IVramManager& vramManager,
    const unsigned int* tiles, u32 tilesLength) const
{
    u32 vramOffset = vramManager.Alloc(tilesLength);
    dma_ntrCopy32(3, tiles, vramManager.GetVramAddress(vramOffset), tilesLength);
    return vramOffset;
}

void DisplaySettingsBottomSheetView::ChangeLanguage(int newIdx)
{
    EnsureLanguagesLoaded();
    if (_languageCount <= 0)
        return;
    
    _selectedLanguageIdx = newIdx;
    _pendingLanguageName = _languageEntries[_selectedLanguageIdx].fileName;
    UpdateLanguageUI();

    /*const auto& langPack = _languagePackService->GetLanguagePack();

    _titleLabel.SetText(langPack.displaySettings_title.GetString());
    _layoutLabel.SetText(langPack.displaySettings_layout.GetString());
    _sortingLabel.SetText(langPack.displaySettings_sorting.GetString());
    _themeLabel.SetText(langPack.displaySettings_theme.GetString());
    _languageLabel.SetText(langPack.displaySettings_langugage.GetString());*/
}

void DisplaySettingsBottomSheetView::EnsureThemesLoaded()
{
    if (_themesLoaded)
        return;

    LoadThemes();
    _themesLoaded = true;
    _selectedThemeIdx = 0;

    for (int i = 0; i < _themeCount; ++i)
    {
        if (strcasecmp(_pendingThemeName.GetString(), _themeNames[i].GetString()) == 0)
        {
            _selectedThemeIdx = i;
            break;
        }
    }

    _pendingThemeName = _themeNames[_selectedThemeIdx];
    UpdateThemeUI();
}

void DisplaySettingsBottomSheetView::LoadThemes()
{
    _themeCount = 0;
    Directory directory;
    if (directory.Open("/_pico/themes") == FR_OK)
    {
        FILINFO fileInfo;
        
        while (true)
        {
            if (directory.Read(&fileInfo) != FR_OK)
                break;
            if (fileInfo.fname[0] == 0)
                break;
            if (fileInfo.fname[0] == '.')
                continue;
            if ((fileInfo.fattrib & AM_DIR) == 0)
                continue;
            if (_themeCount >= kMaxThemeCount)
                break;

            _themeNames[_themeCount++] = fileInfo.fname;
        }
    }

    if (_themeCount == 0)
    {
        _themeNames[0] = "material";
        _themeCount = 1;
    }
}

void DisplaySettingsBottomSheetView::LoadLanguages()
{
    _languageCount = 0;
    Directory directory;
    if (directory.Open("/_pico/lang") != FR_OK)
    {
        _languageEntries[0].fileName = "english";
        _languageCount = 1;
    }
    else
    {
        FILINFO fileInfo;
        while (true)
        {
            if (directory.Read(&fileInfo) != FR_OK)
                break;
            if (fileInfo.fname[0] == 0)
                break;
            if (fileInfo.fname[0] == '.')
                continue;
            if (fileInfo.fattrib & AM_DIR)
                continue;
            if (_languageCount >= kMaxLanguageCount)
                break;

            const char* dot = strrchr(fileInfo.fname, '.');
            if (!dot || strcasecmp(dot, ".json") != 0)
                continue;

            char baseName[64];
            size_t len = (size_t)(dot - fileInfo.fname);
            if (len >= sizeof(baseName))
                len = sizeof(baseName) - 1;
            memcpy(baseName, fileInfo.fname, len);
            baseName[len] = '\0';

            auto& entry = _languageEntries[_languageCount];
            entry.fileName = baseName;
            for (size_t i = 0; i < len && i < 63; ++i)
                entry.displayName[i] = (char16_t)baseName[i];
            entry.displayName[len < 63 ? len : 63] = 0;
            _languageCount++;
        }
    }

    // If no languages found, add a default English entry
    if (_languageCount == 0)
    {
        _languageEntries[0].fileName = "english";
        StringUtil::Copy(_languageEntries[0].displayName, u"english", 64);
        _languageCount = 1;
    }
}

void DisplaySettingsBottomSheetView::EnsureLanguagesLoaded()
{
    if (_languagesLoaded)
        return;

    LoadLanguages();
    _languagesLoaded = true;
    _selectedLanguageIdx = 0;

    for (int i = 0; i < _languageCount; ++i)
    {
        if (strcasecmp(_pendingLanguageName.GetString(), _languageEntries[i].fileName.GetString()) == 0)
        {
            _selectedLanguageIdx = i;
            break;
        }
    }

    _pendingLanguageName = _languageEntries[_selectedLanguageIdx].fileName;
    UpdateLanguageUI();
}

void DisplaySettingsBottomSheetView::ChangeTheme(int newIdx)
{
    EnsureThemesLoaded();
    if (_themeCount <= 0)
        return;

    _selectedThemeIdx = newIdx;
    _pendingThemeName = _themeNames[_selectedThemeIdx];
    UpdateThemeUI();
}

void DisplaySettingsBottomSheetView::UpdateThemeUI()
{
    _themeLabel.SetPosition(THEME_LABEL_X, _position.y + THEME_LABEL_Y);
    _themeFieldLabel.SetPosition(THEME_FIELD_X, _position.y + THEME_LABEL_Y);
    _themeFieldLabel.SetText(_themesLoaded
        ? _themeNames[_selectedThemeIdx].GetString()
        : _pendingThemeName.GetString());
}

void DisplaySettingsBottomSheetView::UpdateLanguageUI()
{
    _languageLabel.SetPosition(LANGUAGE_LABEL_X, _position.y + LANGUAGE_LABEL_Y);
    _languageFieldLabel.SetPosition(LANGUAGE_FIELD_X, _position.y + LANGUAGE_LABEL_Y);
    if (_languagesLoaded && _languageCount > 0)
        _languageFieldLabel.SetText(_languageEntries[_selectedLanguageIdx].displayName);
    else
        _languageFieldLabel.SetText(_pendingLanguageName.GetString());
}

void DisplaySettingsBottomSheetView::ReleaseLazyLists()
{
    _themesLoaded = false;
    _themeCount = 0;
    _selectedThemeIdx = 0;

    _languagesLoaded = false;
    _languageCount = 0;
    _selectedLanguageIdx = 0;
}

IconButton2DView DisplaySettingsBottomSheetView::CreateArrowIcon()
{
    IconButton2DView arrowIcon
    {
        IconButtonView::Type::Standard, // Standard: 배경 없이 아이콘만 깔끔하게 나옵니다.
        IconButtonView::State::ToggleUnselected,
        md::sys::color::surfaceContainerLow,
        _materialColorScheme
    };
    return arrowIcon;
}