#pragma once
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"
#include "../viewModels/RomBrowserViewModel.h"
#include "services/settings/AppSettings.h"

class IFontRepository;
class IAppSettingsService;
class ILanguagePackService;
class MaterialColorScheme;

class LaunchSettingsBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(LaunchSettingsBottomSheetView)
public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenDown(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenMove(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(_loaderFieldLabel);
    }

private:
    RomBrowserViewModel* _viewModel;
    IAppSettingsService* _appSettingsService;
    ILanguagePackService* _languagePackService;

    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<Label2DView> _loaderLabel;
    SharedPtr<Label2DView> _loaderFieldLabel;

    const MaterialColorScheme* _materialColorScheme;
    bool _oobPenDown = false;

    LoaderType _pendingLoaderType;

    u32 _leftArrowIconVramOffset = 0;
    u32 _rightArrowIconVramOffset = 0;

    SharedPtr<IconButton2DView> _loaderLeftArrow;
    SharedPtr<IconButton2DView> _loaderRightArrow;

    SharedPtr<IconButton2DView> CreateArrowIcon();

    
    LaunchSettingsBottomSheetView(RomBrowserViewModel* viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
        IAppSettingsService* appSettingsService, ILanguagePackService* languagePackService);

    void UpdateLabels();
    void ToggleLoaderType();
    u32 LoadIcon(IVramManager& vramManager, const unsigned int* tiles, u32 tilesLength) const;
};