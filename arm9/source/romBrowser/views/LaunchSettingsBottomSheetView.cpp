#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "services/Language/ILanguagePackService.h"
#include "LaunchSettingsBottomSheetView.h"

#define TITLE_LABEL_X       20
#define TITLE_LABEL_Y       16

#define LOADER_FIELD_X      20
#define LOADER_FIELD_Y      60

LaunchSettingsBottomSheetView::LaunchSettingsBottomSheetView(
    SharedPtr<LaunchSettingsViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository, const ILanguagePackService* languagePackService)
    : _viewModel(std::move(viewModel))
    , _languagePack(&languagePackService->GetLanguagePack())
    , _materialColorScheme(materialColorScheme)
    , _titleLabel(Label2DView::CreateShared(128, 16, 25, fontRepository->GetFont(FontType::Medium11)))
    , _loaderField(SelectorFieldView::CreateShared(fontRepository, materialColorScheme))
{
    _titleLabel->SetText(_languagePack->launchSettings_title.GetString());
    _loaderField->SetLabel(_languagePack->launchSettings_loader.GetString());
    _loaderField->SetSelectAction([] (int direction, void* arg)
    {
        auto self = reinterpret_cast<LaunchSettingsBottomSheetView*>(arg);
        if (direction < 0)
            self->_viewModel->SelectPreviousLoader();
        else
            self->_viewModel->SelectNextLoader();
        self->UpdateLoaderText();
    }, this);
    UpdateLoaderText();

    AddChildTail(_titleLabel.GetPointer());
    AddChildTail(_loaderField.GetPointer());
}

void LaunchSettingsBottomSheetView::SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken,
    const SelectorFieldView::VramToken& selectorFieldVramToken)
{
    _loaderField->SetGraphics(iconButtonVramToken, selectorFieldVramToken);
}

void LaunchSettingsBottomSheetView::Close()
{
    _viewModel->Close();
}

void LaunchSettingsBottomSheetView::UpdateLoaderText()
{
    const auto loader = _viewModel->GetLoader();
    if (!loader)
        _loaderField->SetValue(_languagePack->launchSettings_default.GetString());
    else if (*loader == LoaderType::NdsBootstrap)
        _loaderField->SetValue("NDS-Bootstrap");
    else
        _loaderField->SetValue("Pico Loader");
}

void LaunchSettingsBottomSheetView::Update()
{
    BottomSheetView::Update();
    _titleLabel->SetPosition(TITLE_LABEL_X, _position.y + TITLE_LABEL_Y);
    _loaderField->SetPosition(LOADER_FIELD_X, _position.y + LOADER_FIELD_Y);
}

void LaunchSettingsBottomSheetView::Draw(GraphicsContext& graphicsContext)
{
    graphicsContext.SetClipArea(GetBounds());
    u32 oldPrio = graphicsContext.SetPriority(1);
    {
        _titleLabel->SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _titleLabel->SetForegroundColor(_materialColorScheme->onSurface);
        BottomSheetView::Draw(graphicsContext);
    }
    graphicsContext.SetPriority(oldPrio);
    graphicsContext.ResetClipArea();
}

bool LaunchSettingsBottomSheetView::HandleInput(
    const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::A))
    {
        _viewModel->Apply();
        return true;
    }

    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->Close();
        return true;
    }
    return false;
}
