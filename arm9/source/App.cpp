#include "common.h"
#include <algorithm>
#include <libtwl/mem/memVram.h>
#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxOam.h>
#include <libtwl/gfx/gfxPalette.h>
#include <libtwl/gfx/gfxBackground.h>
#include <libtwl/gfx/gfxStatus.h>
#include <libtwl/gfx/gfx3d.h>
#include <libtwl/gfx/gfx3dCmd.h>
#include <libtwl/sys/sysPower.h>
#include "animation/Animator.h"
#include "gui/materialDesign.h"
#include "themes/material/MaterialColorSchemeFactory.h"
#include "core/math/ColorConverter.h"
#include "core/math/RgbMixer.h"
#include "gui/GraphicsContext.h"
#include "romBrowser/views/ChipView.h"
#include "picoLoaderBootstrap.h"
#include "romBrowser/DisplayMode/RomBrowserDisplayModeFactory.h"
#include "romBrowser/Theme/Material/MaterialThemeFileIconFactory.h"
#include "romBrowser/views/NdsGameDetailsBottomSheetView.h"
#include "romBrowser/views/cheats/CheatsBottomSheetView.h"
#include "romBrowser/views/DisplaySettingsBottomSheetView.h"
#include "romBrowser/views/LaunchSettingsBottomSheetView.h"
#include "bgm/AudioStreamPlayer.h"
#include "bgm/BgmService.h"
#include "themes/ThemeInfoFactory.h"
#include "themes/ThemeFactory.h"
#include "gui/Gx.h"
#include "splashTop.h"
#include "App.h"
#include "PicoLoaderProcess.h"
#include "NdsBootstrap/NdsBootstrapProcess.h"

#define SPLASH_FRAMES       44

static bool TryGetThemeReloadLauncherPath(const char*& outLauncherPath)
{
    FILINFO fileInfo;
    if (f_stat("/_picoboot.nds", &fileInfo) == FR_OK && (fileInfo.fattrib & AM_DIR) == 0)
    {
        outLauncherPath = "/_picoboot.nds";
        return true;
    }

    outLauncherPath = nullptr;
    return false;
}

App::App(IAppSettingsService& appSettingsService, IBgmService& bgmService, ILanguagePackService& languagePackService)
    : _mainObjPltt(GFX_PLTT_OBJ_MAIN)
    , _mainObjVram(GFX_OBJ_MAIN)
    , _mainObjDialogVram(GFX_OBJ_MAIN, 128 * 1024)
    , _subObjVram(GFX_OBJ_SUB)
    , _textureVram((vu16*)0x06860000)
    , _texturePaletteVram((vu16*)0x6880000)
    , _mainVramContext(nullptr, &_mainObjVram, &_textureVram, &_texturePaletteVram)
    , _subVramContext(nullptr, &_subObjVram, nullptr, nullptr)
    , _appSettingsService(appSettingsService)
    , _bgmService(bgmService)
    , _languagePackService(languagePackService)
    , _inputProvider(&_keyInputSource, &_touchInputSource)
    , _inputRepeater(&_inputProvider,
        InputKey::DpadLeft | InputKey::DpadRight | InputKey::DpadUp | InputKey::DpadDown | InputKey::L | InputKey::R,
        25, 8)
    , _romBrowserController(&appSettingsService, &_ioTaskQueue, &_bgTaskQueue)
    , _displaySettingsBottomSheetViewModel(&_romBrowserController)
    , _romBrowserBottomScreenViewModel(&_romBrowserController)
    , _dialogPresenter(&_focusManager, &_mainObjDialogVram) { }

void App::InitVramMapping() const
{
    mem_setVramAMapping(MEM_VRAM_AB_TEX_SLOT_1);
    mem_setVramBMapping(MEM_VRAM_AB_MAIN_OBJ_00000);
    mem_setVramCMapping(MEM_VRAM_C_SUB_BG_00000);
    mem_setVramDMapping(MEM_VRAM_D_TEX_SLOT_0);
    mem_setVramEMapping(MEM_VRAM_E_TEX_PLTT_SLOT_0123);
    mem_setVramFMapping(MEM_VRAM_FG_MAIN_BG_00000);
    mem_setVramGMapping(MEM_VRAM_FG_MAIN_BG_04000);
    mem_setVramHMapping(MEM_VRAM_H_SUB_BG_EXT_PLTT_SLOT_0123);
    mem_setVramIMapping(MEM_VRAM_I_SUB_OBJ_00000);
}

void App::DisplaySplashScreen() const
{
    dma_ntrCopy32(3, splashTopTiles, GFX_BG_SUB, splashTopTilesLen);
    dma_ntrCopy32(3, splashTopMap, (u8*)GFX_BG_SUB + 0x3000, splashTopMapLen);
    mem_setVramHMapping(MEM_VRAM_H_LCDC);
    dma_ntrCopy32(3, splashTopPal, (void*)0x0689A000, splashTopPalLen);
    mem_setVramHMapping(MEM_VRAM_H_SUB_BG_EXT_PLTT_SLOT_0123);

    VBlank::Wait();

    sys_setMainEngineToBottomScreen();
    REG_DISPCNT_SUB = 0x40211015;
    REG_BG1HOFS_SUB = 0;
    REG_BG1VOFS_SUB = 0;
    REG_BG1CNT_SUB = 0x0680;
    REG_DISPCNT_SUB |= 1 << 9;
    REG_BLDCNT_SUB = 0x3D42;
    REG_BLDALPHA_SUB = 0x10;
    REG_MASTER_BRIGHT_SUB = 0;
}

void App::LoadTheme()
{
    ThemeInfoFactory themeInfoFactory;
    auto themeInfo = themeInfoFactory.CreateFromThemeFolder(_appSettingsService.GetAppSettings().theme);
    if (!themeInfo)
    {
        LOG_DEBUG("Failed to load theme '%s'. Using fallback theme.\n", _appSettingsService.GetAppSettings().theme.GetString());
        themeInfo = themeInfoFactory.CreateFallbackTheme();
    }
    _theme = ThemeFactory().CreateFromThemeInfo(themeInfo.get());
    themeInfo.reset();
    _theme->LoadRomBrowserResources(_mainVramContext, _subVramContext);
    _topBackground = _theme->CreateRomBrowserTopBackground();
    _topBackground->LoadResources(*_theme, _subVramContext);
    _bottomBackground = _theme->CreateRomBrowserBottomBackground();
    _bottomBackground->LoadResources(*_theme, _mainVramContext);

    _materialThemeFileIconFactory = std::make_unique<MaterialThemeFileIconFactory>(
        &_theme->GetMaterialColorScheme(), _theme->GetFontRepository());
}

void App::VCountIrq()
{
    _mainObjPltt.VCount();
}

void App::Run()
{
    InitVramMapping();
    DisplaySplashScreen();
    gx_init();

    _chipViewVram = ChipView::UploadGraphics(_mainObjVram);
    _iconButtonViewVram = IconButton2DView::UploadGraphics(_mainObjVram);

    mem_setVramEMapping(MEM_VRAM_E_LCDC);
    _rgb6Palette.UploadGraphics(_mainVramContext);
    mem_setVramEMapping(MEM_VRAM_E_TEX_PLTT_SLOT_0123);

    _dialogPresenter.InitVram();

    LoadTheme();

    _ioTaskQueue.StartThread(1, _ioTaskThreadStack, sizeof(_ioTaskThreadStack));
    _bgTaskQueue.StartThread(2, _bgTaskThreadStack, sizeof(_bgTaskThreadStack));

    StoreVramState(_vramStateBeforeMakeBottomScreenView);

    _romBrowserBottomScreenView = RomBrowserBottomScreenView::CreateShared(
        &_romBrowserBottomScreenViewModel,
        RomBrowserDisplayModeFactory().GetRomBrowserDisplayMode(
            _romBrowserController.GetRomBrowserDisplaySettings().layout),
        _materialThemeFileIconFactory.get(),
        _theme->GetRomBrowserViewFactory(),
        &_vblankTextureLoader);
    _romBrowserBottomScreenView->InitVram(_mainVramContext);

    _loadingView = std::make_unique<LoadingView>(
        &_loadingViewModel,
        _theme->GetFontRepository(),
        &_theme->GetMaterialColorScheme());

    _loadingView->InitVram(_mainVramContext);

    StoreVramState(_vramStateAfterMakeBottomScreenView);

    const auto& materialColorScheme = _theme->GetMaterialColorScheme();

    auto scrimBlendColor = Rgb<8, 8, 8>(
        materialColorScheme.inverseOnSurface.r + (materialColorScheme.scrim.r - materialColorScheme.inverseOnSurface.r) * 5 / 16,
        materialColorScheme.inverseOnSurface.g + (materialColorScheme.scrim.g - materialColorScheme.inverseOnSurface.g) * 5 / 16,
        materialColorScheme.inverseOnSurface.b + (materialColorScheme.scrim.b - materialColorScheme.inverseOnSurface.b) * 5 / 16);

    RgbMixer::MakeGradientPalette((u16*)GFX_PLTT_BG_MAIN, scrimBlendColor, materialColorScheme.GetColor(md::sys::color::surfaceContainerLow));

    GFX_PLTT_BG_MAIN[0] = ColorConverter::ToGBGR565(materialColorScheme.inverseOnSurface);
    GFX_PLTT_BG_MAIN[31] = ColorConverter::ToGBGR565(materialColorScheme.scrim);
    REG_DISPCNT = 0x211F1B;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG0CNT = 3;

    Gx::MtxMode(GX_MTX_MODE_PROJECTION);
    mtx43_t orthoMtx =
    {
        2048, 0, 0,
        0, -21845, 0,
        0, 0, 4096 >> 5,
        -4096, 4096, 0
    };
    Gx::MtxLoad43(&orthoMtx);

    _vcountIrqStarted = false;
    rtos_disableIrqMask(RTOS_IRQ_VCOUNT);
    rtos_setIrqFunc(RTOS_IRQ_VCOUNT, [] (u32 mask) { ((App*)gProcessManager.GetRunningProcess())->VCountIrq(); });

    LOG_DEBUG("Amount of main obj vram used: %d\n", _mainObjVram.GetState());

    _ioTaskQueue.Enqueue([this] (const vu8& cancelRequested)
    {
        _bgmService.StartBgmFromConfig();
        return TaskResult<void>::Completed();
    });

    _fadeAnimator = Animator(16, 0, 16, &md::sys::motion::easing::linear);

    MainLoop();

    if(!_isBgmStoppedForLaunch) _bgmService.StopBgm();
    rtos_disableIrqMask(RTOS_IRQ_VCOUNT);
    rtos_setIrqFunc(RTOS_IRQ_VCOUNT, nullptr);
}

void App::MainLoop()
{
    bool fadeIn = true;
    int fadeWaitFrames = SPLASH_FRAMES;
    while (true)
    {
        Update();
        Draw();
        VBlank::Wait();
        VBlank();
        if (_exit)
        {
            bool fadeComplete = _fadeAnimator.Update();
            REG_MASTER_BRIGHT = 0x4000 | _fadeAnimator.GetValue();
            REG_MASTER_BRIGHT_SUB = 0x4000 | _fadeAnimator.GetValue();
            if (fadeComplete)
            {
                break;
            }
        }
        else if (fadeIn)
        {
            if (fadeWaitFrames)
            {
                fadeWaitFrames--;
                REG_BLDALPHA_SUB = 16;
                REG_MASTER_BRIGHT = 0x4010;
            }
            else
            {
                bool fadeComplete = _fadeAnimator.Update();
                if (fadeComplete)
                {
                    fadeIn = false;
                    REG_BLDCNT_SUB = 0;
                    REG_DISPCNT_SUB &= ~(1 << 9);
                    REG_MASTER_BRIGHT = 0;
                }
                else
                {
                    int fade = _fadeAnimator.GetValue();
                    REG_BLDALPHA_SUB = ((16 - fade) << 8) | fade;
                    REG_MASTER_BRIGHT = 0x4000 | fade;
                }
            }
        }
    }
}

void App::Exit()
{
    _fadeAnimator.Goto(16, 16, &md::sys::motion::easing::linear);
    _exit = true;
}

void App::HandleTrigger(RomBrowserStateTrigger trigger, RomBrowserState newState)
{
    switch (trigger)
    {
        case RomBrowserStateTrigger::None:
        case RomBrowserStateTrigger::Launch:
        {
            _launchDelayFrames = 2;
            break;
        }
        case RomBrowserStateTrigger::ShowGameInfo:
        {
            HandleShowGameInfoTrigger();
            break;
        }
        case RomBrowserStateTrigger::HideGameInfo:
        {
            HandleHideGameInfoTrigger();
            break;
        }
        case RomBrowserStateTrigger::ShowDisplaySettings:
        {
            HandleShowDisplaySettingsTrigger();
            break;
        }
        case RomBrowserStateTrigger::HideDisplaySettings:
        {
            HandleHideDisplaySettingsTrigger();
            break;
        }
        case RomBrowserStateTrigger::Navigate:
        {
            HandleNavigateTrigger();
            break;
        }
        case RomBrowserStateTrigger::FolderLoadDone:
        {
            HandleFolderLoadDoneTrigger();
            break;
        }
        case RomBrowserStateTrigger::ChangeDisplayMode:
        {
            _changeDisplayMode = true;
            break;
        }
        case RomBrowserStateTrigger::ShowLaunchSettings:
        {
            HandleShowLaunchSettingsTrigger();
            break;
        }
        case RomBrowserStateTrigger::HideLaunchSettings:
        {
            HandleHideLaunchSettingsTrigger();
            break;
        }
    }
}

void App::HandleShowGameInfoTrigger()
{
    // auto gameInfoDialog = std::make_unique<NdsGameDetailsBottomSheetView>(
    //     &_romBrowserController, &_theme->GetMaterialColorScheme(), _theme->GetFontRepository());
    // gameInfoDialog->SetGraphics(_chipViewVram);
    // _dialogPresenter.ShowDialog(std::move(gameInfoDialog));

    auto cheatsViewModel = SharedPtr<CheatsViewModel>::MakeShared(_romBrowserController.GetTriggerFileInfo(), &_romBrowserController);
    auto cheatsDialog = CheatsBottomSheetView::CreateShared(
        std::move(cheatsViewModel), &_theme->GetMaterialColorScheme(), _theme->GetFontRepository(), &_focusManager, &_languagePackService);
    _dialogPresenter.ShowDialog(std::move(cheatsDialog));
}

void App::HandleHideGameInfoTrigger()
{
    _dialogPresenter.CloseDialog();
    if (!_dialogPresenter.GetOldFocus())
        _romBrowserBottomScreenView->Focus(_focusManager);
}

void App::HandleShowDisplaySettingsTrigger()
{
    auto displaySettingsDialog = DisplaySettingsBottomSheetView::CreateShared(
        &_displaySettingsBottomSheetViewModel, &_theme->GetMaterialColorScheme(), _theme->GetFontRepository(), &_appSettingsService, &_languagePackService);
    displaySettingsDialog->SetGraphics(_iconButtonViewVram);
    _dialogPresenter.ShowDialog(std::move(displaySettingsDialog));
}

void App::HandleHideDisplaySettingsTrigger()
{
    _dialogPresenter.CloseDialog();

    if (_romBrowserController.ConsumeThemeReloadRequest())
    {
        _pendingAppRestart = true;
    }

    if (!_dialogPresenter.GetOldFocus())
        _romBrowserBottomScreenView->Focus(_focusManager);
}

void App::HandleShowLaunchSettingsTrigger()
{
    auto launchSettingsDialog = LaunchSettingsBottomSheetView::CreateShared(
        _romBrowserController.GetRomBrowserViewModel().GetPointer(), 
        &_theme->GetMaterialColorScheme(), 
        _theme->GetFontRepository(), 
        &_appSettingsService, 
        &_languagePackService);
        
    _dialogPresenter.ShowDialog(std::move(launchSettingsDialog));
}

void App::HandleHideLaunchSettingsTrigger()
{
    _dialogPresenter.CloseDialog();
    
    if (!_dialogPresenter.GetOldFocus())
        _romBrowserBottomScreenView->Focus(_focusManager);
}

void App::HandleNavigateTrigger()
{
    if (!_romBrowserBottomScreenView->IsAppBarFocused(_focusManager))
        _focusManager.Unfocus();
}

void App::HandleFolderLoadDoneTrigger()
{
    _romBrowserTopScreenView.Reset();
    RestoreVramState(_vramStateAfterMakeBottomScreenView);
    auto displayMode = RomBrowserDisplayModeFactory().GetRomBrowserDisplayMode(
        _romBrowserController.GetRomBrowserDisplaySettings().layout);
    _romBrowserTopScreenView = RomBrowserTopScreenView::CreateShared(
        _romBrowserController.GetRomBrowserViewModel(),
        displayMode,
        _materialThemeFileIconFactory.get(),
        _theme->GetRomBrowserViewFactory());
    _romBrowserTopScreenView->InitVram(_subVramContext);
    _romBrowserBottomScreenView->RomBrowserViewModelInvalidated(_mainVramContext);
    if (!_focusManager.GetCurrentFocus())
        _romBrowserBottomScreenView->Focus(_focusManager);
}

void App::HandleChangeDisplayModeTrigger(RomBrowserState newState)
{
    _dialogPresenter.ClearOldFocus();
    RestoreVramState(_vramStateBeforeMakeBottomScreenView);
    auto displayMode = RomBrowserDisplayModeFactory().GetRomBrowserDisplayMode(
        _romBrowserController.GetRomBrowserDisplaySettings().layout);
    _romBrowserBottomScreenView = RomBrowserBottomScreenView::CreateShared(
        &_romBrowserBottomScreenViewModel,
        displayMode,
        _materialThemeFileIconFactory.get(),
        _theme->GetRomBrowserViewFactory(),
        &_vblankTextureLoader);
    _romBrowserBottomScreenView->InitVram(_mainVramContext);
    StoreVramState(_vramStateAfterMakeBottomScreenView);
    _romBrowserTopScreenView = RomBrowserTopScreenView::CreateShared(
        _romBrowserController.GetRomBrowserViewModel(),
        displayMode,
        _materialThemeFileIconFactory.get(),
        _theme->GetRomBrowserViewFactory());
    _romBrowserTopScreenView->InitVram(_subVramContext);
    _romBrowserBottomScreenView->RomBrowserViewModelInvalidated(_mainVramContext);
    if (newState == RomBrowserState::Browser)
        _romBrowserBottomScreenView->Focus(_focusManager);
}

bool App::IsRomBrowserVisible() const
{
    const auto& stateMachine = _romBrowserController.GetStateMachine();
    auto curState = stateMachine.GetCurrentState();
    return curState == RomBrowserState::Browser
        || curState == RomBrowserState::GameInfo
        || curState == RomBrowserState::DisplaySettings
        || curState == RomBrowserState::ShowLaunchSettings
        || curState == RomBrowserState::Launching;
}

void App::Update()
{
    const auto& stateMachine = _romBrowserController.GetStateMachine();
    _romBrowserController.Update();
    auto curState = stateMachine.GetCurrentState();
    if (_changeDisplayMode)
    {
        HandleChangeDisplayModeTrigger(curState);
        _changeDisplayMode = false;
    }
    if (stateMachine.HasStateChanged())
    {
        HandleTrigger(stateMachine.GetLastTrigger(), curState);
    }

    bool isRomBrowserVisible = IsRomBrowserVisible();
    if (isRomBrowserVisible && !_exit && curState != RomBrowserState::Launching)
    {
        HandleInput();
    }

    if (_topBackground)
        _topBackground->Update();
    if (_bottomBackground)
        _bottomBackground->Update();

    _dialogPresenter.Update();
    
    if (_pendingAppRestart && _dialogPresenter.IsIdle())
    {
        _pendingAppRestart = false;

        const char* launcherPath = nullptr;
        if (!TryGetThemeReloadLauncherPath(launcherPath))
        {
            return;
        }

        auto loadParams = pload_getLoadParams();
        StringUtil::Copy(loadParams->romPath, launcherPath, sizeof(loadParams->romPath));
        loadParams->savePath[0] = 0;
        loadParams->arguments[0] = 0;
        loadParams->argumentsLength = 0;
        pload_setCheatData(nullptr);
        gProcessManager.Goto<PicoLoaderProcess>();
        return;
    }

    bool isNdsBootstrap = (_appSettingsService.GetAppSettings().loaderType == LoaderType::NDS_Bootstrap);
    bool showLoadingView = (curState == RomBrowserState::Launching && isNdsBootstrap);

    _romBrowserBottomScreenView->Update();

    if (IsRomBrowserVisible())
    {
        _romBrowserTopScreenView->Update();
        _romBrowserController.GetRomBrowserViewModel()->SetIconFrameCounter(
            _romBrowserController.GetRomBrowserViewModel()->GetIconFrameCounter() + 1);
    }

    if (showLoadingView && _loadingView)
    {
        _loadingView->Update();
        LOG_DEBUG("Loading view frame counter: %d\n", _romBrowserController.GetRomBrowserViewModel()->GetIconFrameCounter());
        if(!_isBgmStoppedForLaunch){
            _bgmService.StopBgm();
            _isBgmStoppedForLaunch = true;
        }
    }
}

void App::Draw()
{
    gx_reset();
    Gx::Viewport(0, 0, 255, 191);
    Gx::MtxMode(GX_MTX_MODE_POSITION_VECTOR);
    Gx::MtxIdentity();

    GraphicsContext mainGraphicsContext
    {
        &_mainOam,
        &_mainObjPltt,
        &_rgb6Palette
    };
    GraphicsContext subGraphicsContext
    {
        &_subOam,
        &_subObjPltt,
        nullptr
    };

    _mainOam.Clear();
    _subOam.Clear();
    _mainObjPltt.Reset();
    _subObjPltt.Reset();
    mainGraphicsContext.SetPriority(3);
    subGraphicsContext.SetPriority(2);

    if (_topBackground)
        _topBackground->Draw(subGraphicsContext);
    if (_bottomBackground)
        _bottomBackground->Draw(mainGraphicsContext);

    auto curState = _romBrowserController.GetStateMachine().GetCurrentState();
    bool isNdsBootstrap = (_appSettingsService.GetAppSettings().loaderType == LoaderType::NDS_Bootstrap);
    bool showLoadingView = (curState == RomBrowserState::Launching && isNdsBootstrap);

    if (!_changeDisplayMode && IsRomBrowserVisible())
    {
        _romBrowserTopScreenView->Draw(subGraphicsContext);
    }

    _dialogPresenter.ApplyClipArea(mainGraphicsContext);
    if (!_changeDisplayMode)
    {
        _romBrowserBottomScreenView->Draw(mainGraphicsContext);

        if (showLoadingView && _loadingView)
        {
            _loadingView->Draw(mainGraphicsContext);
        }
    }
    mainGraphicsContext.ResetClipArea();

    _dialogPresenter.Draw(mainGraphicsContext);

    _mainObjPltt.EndOfFrame();

    Gx::SwapBuffers(GX_XLU_SORT_MANUAL, GX_DEPTH_MODE_Z);
}

void App::VBlank()
{
    dma_ntrStopDirect(0); // stop hblank dma
    _inputProvider.Sample();
    _inputRepeater.Update();
    _mainOam.Apply(GFX_OAM_MAIN);
    _subOam.Apply(GFX_OAM_SUB);
    _subObjPltt.Apply(GFX_PLTT_OBJ_SUB);

    if (!_vcountIrqStarted)
    {
        rtos_ackIrqMask(RTOS_IRQ_VCOUNT);
        rtos_enableIrqMask(RTOS_IRQ_VCOUNT);
        _vcountIrqStarted = true;
    }
    _mainObjPltt.VBlank();

    if (_topBackground)
        _topBackground->VBlank();
    if (_bottomBackground)
        _bottomBackground->VBlank();

    auto curState = _romBrowserController.GetStateMachine().GetCurrentState();
    bool isNdsBootstrap = (_appSettingsService.GetAppSettings().loaderType == LoaderType::NDS_Bootstrap);
    bool showLoadingView = (curState == RomBrowserState::Launching && isNdsBootstrap);

    _dialogPresenter.VBlank();

    if (IsRomBrowserVisible())
    {
        _romBrowserTopScreenView->VBlank();
    }
    
    _romBrowserBottomScreenView->VBlank();

    if (showLoadingView && _loadingView)
    {
        _loadingView->VBlank();
    }

    _vblankTextureLoader.VBlank();
}

void App::StoreVramState(VramState& vramState) const
{
    vramState._mainObjVramState = _mainObjVram.GetState();
    vramState._texVramState = _textureVram.GetState();
    vramState._texPlttVramState = _texturePaletteVram.GetState();
    vramState._subObjVramState = _subObjVram.GetState();
}

void App::RestoreVramState(const VramState& vramState)
{
    _mainObjVram.SetState(vramState._mainObjVramState);
    _textureVram.SetState(vramState._texVramState);
    _texturePaletteVram.SetState(vramState._texPlttVramState);
    _subObjVram.SetState(vramState._subObjVramState);
}

void App::HandleInput()
{
    _focusManager.Update(_inputRepeater);
    auto dialogView = _dialogPresenter.GetDialogView();
    Point touchPoint;
    if (_inputRepeater.Triggered(InputKey::Touch) &&
        _inputRepeater.GetCurrentTouchPoint(touchPoint))
    {
        // pen down
        if (_dialogPresenter.IsBottomSheetVisible() && dialogView != nullptr)
        {
            dialogView->HandlePenDown(touchPoint, _focusManager);
        }
        else
        {
            _romBrowserBottomScreenView->HandlePenDown(touchPoint, _focusManager);
        }
        _lastTouchPoint = touchPoint;
    }
    else if (_inputRepeater.Released(InputKey::Touch))
    {
        // pen up
        if (_dialogPresenter.IsBottomSheetVisible() && dialogView != nullptr)
        {
            dialogView->HandlePenUp(_lastTouchPoint, _focusManager);
        }
        else
        {
            _romBrowserBottomScreenView->HandlePenUp(_lastTouchPoint, _focusManager);
        }
    }
    else if (_inputRepeater.Current(InputKey::Touch)
        && _inputRepeater.GetCurrentTouchPoint(touchPoint))
    {
        // pen move
        if (_dialogPresenter.IsBottomSheetVisible())
        {
            dialogView->HandlePenMove(touchPoint, _focusManager);
        }
        else
        {
            _romBrowserBottomScreenView->HandlePenMove(touchPoint, _focusManager);
        }
        _lastTouchPoint = touchPoint;
    }
}
