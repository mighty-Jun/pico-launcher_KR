#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/VramContext.h"
#include "gui/IVramManager.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "LaunchSettingsBottomSheetView.h"
#include <services/settings/IAppSettingsService.h>
#include <services/Language/ILanguagePackService.h>
#include "left_icon.h"
#include "right_icon.h"

#define TITLE_LABEL_X       20
#define TITLE_LABEL_Y       16

#define LOADER_LABEL_X      20
#define LOADER_LABEL_Y      60

#define LOADER_FIELD_X      100

LaunchSettingsBottomSheetView::LaunchSettingsBottomSheetView(
    RomBrowserViewModel* viewModel, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository, IAppSettingsService* appSettingsService,
    ILanguagePackService* languagePackService)
    : _viewModel(viewModel)
    , _appSettingsService(appSettingsService)
    , _languagePackService(languagePackService)
    , _titleLabel(128, 16, 25, fontRepository->GetFont(FontType::Medium11))
    , _loaderLabel(64, 16, 25, fontRepository->GetFont(FontType::Regular10))
    , _loaderFieldLabel(100, 16, 20, fontRepository->GetFont(FontType::Regular10))
    , _materialColorScheme(materialColorScheme)
{
    // const auto& langPack = _languagePackService->GetLanguagePack();
    // _titleLabel.SetText(langPack.launchSettings_title.GetString());
    // _loaderLabel.SetText(langPack.launchSettings_loader.GetString());
    
    _titleLabel.SetText("Launch Settings");
    _loaderLabel.SetText("Loader");

    _pendingLoaderType = _appSettingsService->GetAppSettings().loaderType;

    _loaderFieldLabel.SetHorizontalAlignment(Alignment::Center);
    _loaderFieldLabel.SetText(_pendingLoaderType == LoaderType::Pico_Loader ? "PicoLoader" : "nds-bootstrap");

    AddChildTail(&_titleLabel);
    AddChildTail(&_loaderLabel);
    AddChildTail(&_loaderFieldLabel);

    _loaderLeftArrow = CreateArrowIcon();
    _loaderRightArrow = CreateArrowIcon();

    AddChildTail(&_loaderLeftArrow);
    AddChildTail(&_loaderRightArrow);
}

IconButton2DView LaunchSettingsBottomSheetView::CreateArrowIcon()
{
    IconButton2DView arrowIcon
    {
        IconButtonView::Type::Standard,
        IconButtonView::State::ToggleUnselected,
        md::sys::color::onPrimary,
        _materialColorScheme
    };
    return arrowIcon;
}

void LaunchSettingsBottomSheetView::InitVram(const VramContext& vramContext)
{
    BottomSheetView::InitVram(vramContext);

    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        _leftArrowIconVramOffset = LoadIcon(*objVramManager, left_iconTiles, left_iconTilesLen);
        _rightArrowIconVramOffset = LoadIcon(*objVramManager, right_iconTiles, right_iconTilesLen);

        _loaderLeftArrow.SetIconVramOffset(_leftArrowIconVramOffset);
        _loaderRightArrow.SetIconVramOffset(_rightArrowIconVramOffset);
    }
}

u32 LaunchSettingsBottomSheetView::LoadIcon(IVramManager& vramManager,
    const unsigned int* tiles, u32 tilesLength) const
{
    u32 vramOffset = vramManager.Alloc(tilesLength);
    dma_ntrCopy32(3, tiles, vramManager.GetVramAddress(vramOffset), tilesLength);
    return vramOffset;
}

void LaunchSettingsBottomSheetView::UpdateLabels()
{
    _titleLabel.SetPosition(TITLE_LABEL_X, _position.y + TITLE_LABEL_Y);
    _loaderLabel.SetPosition(LOADER_LABEL_X, _position.y + LOADER_LABEL_Y);
    _loaderFieldLabel.SetPosition(LOADER_FIELD_X, _position.y + LOADER_LABEL_Y);

    _loaderLeftArrow.SetPosition(LOADER_FIELD_X - 20, _position.y + LOADER_LABEL_Y - 8);
    _loaderRightArrow.SetPosition(LOADER_FIELD_X + 106, _position.y + LOADER_LABEL_Y - 8);
}

void LaunchSettingsBottomSheetView::Update()
{
    BottomSheetView::Update();
    UpdateLabels();
}

void LaunchSettingsBottomSheetView::Draw(GraphicsContext& graphicsContext)
{
    graphicsContext.SetClipArea(GetBounds());
    u32 oldPrio = graphicsContext.SetPriority(1);
    {
        _titleLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _titleLabel.SetForegroundColor(_materialColorScheme->onSurface);
        
        _loaderLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _loaderLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);

        bool loaderFocused = _loaderFieldLabel.IsFocused();
        _loaderFieldLabel.SetBackgroundColor(loaderFocused
            ? _materialColorScheme->GetColor(md::sys::color::secondaryContainer)
            : _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _loaderFieldLabel.SetForegroundColor(loaderFocused
            ? _materialColorScheme->GetColor(md::sys::color::primary)
            : _materialColorScheme->onSurfaceVariant);

        BottomSheetView::Draw(graphicsContext);
    }
    graphicsContext.SetPriority(oldPrio);
    graphicsContext.ResetClipArea();
}

bool LaunchSettingsBottomSheetView::HandleInput(
    const InputProvider& inputProvider, FocusManager& focusManager)
{
    bool loaderFocused = _loaderFieldLabel.IsFocused();
    
    bool isLeftHeld = inputProvider.Current(InputKey::DpadLeft); 
    bool isRightHeld = inputProvider.Current(InputKey::DpadRight);

    _loaderLeftArrow.SetState((loaderFocused && isLeftHeld) ? IconButtonView::State::ToggleSelected : IconButtonView::State::ToggleUnselected);
    _loaderRightArrow.SetState((loaderFocused && isRightHeld) ? IconButtonView::State::ToggleSelected : IconButtonView::State::ToggleUnselected);

    if (loaderFocused && inputProvider.Triggered(InputKey::A))
    {
        _appSettingsService->GetAppSettings().loaderType = _pendingLoaderType;
        
        _viewModel->SaveSettingsNow();
        _viewModel->HideLaunchSettings(); 
        return true;
    }

    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->HideLaunchSettings();
        return true;
    }
    return false;
}

View* LaunchSettingsBottomSheetView::MoveFocus(View* currentFocus,
    FocusMoveDirection direction, View* source)
{
    if (currentFocus == &_loaderFieldLabel)
    {
        if (direction == FocusMoveDirection::Left || direction == FocusMoveDirection::Right)
        {
            ToggleLoaderType();
        }
        return &_loaderFieldLabel;
    }
    return nullptr;
}

void LaunchSettingsBottomSheetView::ToggleLoaderType()
{
    if (_pendingLoaderType == LoaderType::Pico_Loader)
    {
        _pendingLoaderType = LoaderType::NDS_Bootstrap;
        _loaderFieldLabel.SetText("nds-bootstrap");
    }
    else
    {
        _pendingLoaderType = LoaderType::Pico_Loader;
        _loaderFieldLabel.SetText("PicoLoader");
    }
}