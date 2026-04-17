#include "common.h"
#include <libtwl/mem/memVram.h>
#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxBackground.h>
#include <libtwl/gfx/gfxPalette.h>
#include <libtwl/gfx/gfxWindow.h>
#include "../viewModels/RomBrowserViewModel.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "themes/material/MaterialColorScheme.h"
#include "../Theme/IRomBrowserViewFactory.h"
#include "RomBrowserTopScreenView.h"
#include "gui/OamManager.h"
#include "gui/OamBuilder.h"
#include "gui/VramContext.h"
#include "gui/palette/DirectPalette.h"
#include "sharedMemory.h"
#include <nds/system.h>

static int s_lastKnownBatteryFrame = -1;

RomBrowserTopScreenView::RomBrowserTopScreenView(
    SharedPtr<RomBrowserViewModel> viewModel,
    const RomBrowserDisplayMode* displayMode,
    const IThemeFileIconFactory* themeFileIconFactory,
    const IRomBrowserViewFactory* romBrowserViewFactory)
    : _viewModel(std::move(viewModel))
    , _themeFileIconFactory(themeFileIconFactory)
    , _fileInfoView(romBrowserViewFactory->CreateFileInfoView())
    , _showCover(displayMode->ShowCoverOnTopScreen())
    , _coverPosition(romBrowserViewFactory->GetTopCoverPosition())
{
    AddChildTail(_fileInfoView.GetPointer());

    _batteryFrame = s_lastKnownBatteryFrame;
    //UpdateBatteryLevel();
}

void RomBrowserTopScreenView::InitVram(const VramContext& vramContext)
{
    ViewContainer::InitVram(vramContext);
    auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        _batteryVramOffset = objVramManager->Alloc(batteryTilesLen);
        dma_ntrCopy32(3, batteryTiles, objVramManager->GetVramAddress(_batteryVramOffset), batteryTilesLen);
    }

    int tileIndex = 0;
    vu16* mapPtr = (vu16*)((u8*)GFX_BG_SUB + 0x3800);
    for (int y = 0; y < 12; y++)
    {
        for (int x = 0; x < 14; x++)
        {
            *mapPtr++ = tileIndex;
            tileIndex++;
        }
        mapPtr += 2;
    }
}

void RomBrowserTopScreenView::Update()
{
    int selectedItem = _viewModel->GetSelectedItem();
    if (selectedItem != _lastSelectedItem)
    {
        auto& fileInfoManager = _viewModel->GetFileInfoManager();
        const auto& item = fileInfoManager.GetItem(selectedItem);
        if (item.GetFileType()->HasInternalFileInfo())
        {
            auto info = fileInfoManager.GetInternalFileInfo(selectedItem);
            if (info)
            {
                bool fileNameAsTitle = true;
                const char16_t* gameTitle = info->GetGameTitle();
                if (gameTitle)
                {
                    _fileInfoView->SetGameTitleAsync(_viewModel->GetBgTaskQueue(), gameTitle);
                    fileNameAsTitle = false;
                }

                _selectedFileIcon = info->CreateGameIcon();
                if (!_selectedFileIcon)
                {
                    _selectedFileIcon = item.GetFileType()->CreateFileIcon("", _themeFileIconFactory);
                }
                if (_selectedFileIcon)
                {
                    _selectedFileIcon->SetAnimFrame(_viewModel->GetIconFrameCounter());
                    _iconGraphicsUploaded = false;
                }
                _fileInfoView->SetIcon(std::move(_selectedFileIcon));
                _fileInfoView->SetFileNameAsync(_viewModel->GetBgTaskQueue(), item.GetFileName(), fileNameAsTitle);

                _lastSelectedItem = selectedItem;

                auto cover = fileInfoManager.GetFileCover(selectedItem);
                if (cover.IsValid())
                {
                    _selectedFileCover = std::move(cover);
                    _coverGraphicsUploaded = false;
                }
            }
        }
        else
        {
            auto cover = fileInfoManager.GetFileCover(selectedItem);
            if (cover.IsValid())
            {
                _selectedFileCover = std::move(cover);
                _coverGraphicsUploaded = false;

                _selectedFileIcon = item.GetFileType()->CreateFileIcon("", _themeFileIconFactory);
                if (_selectedFileIcon)
                {
                    _selectedFileIcon->SetAnimFrame(_viewModel->GetIconFrameCounter());
                    _iconGraphicsUploaded = false;
                }
                _fileInfoView->SetIcon(std::move(_selectedFileIcon));
                _fileInfoView->SetFileNameAsync(_viewModel->GetBgTaskQueue(), item.GetFileName(), true);

                _lastSelectedItem = selectedItem;
            }
        }
    }

    _batteryCheckTimer++;
    if (_batteryCheckTimer >= 60)
    {
        _batteryCheckTimer = 0;
        UpdateBatteryLevel(); 
    }
    
    ViewContainer::Update();
}

void RomBrowserTopScreenView::VBlank()
{
    ViewContainer::VBlank();

    if (!_coverGraphicsUploaded && _selectedFileCover.IsValid())
    {
        if (_showCover && _selectedFileCover->IsActualCover())
        {
            _selectedFileCover->Upload2DCoverBitmap((u8*)GFX_BG_SUB + 0x4000);
            mem_setVramHMapping(MEM_VRAM_H_LCDC);
            _selectedFileCover->Upload2DCoverPalette((void*)0x0689E000);
            GFX_PLTT_BG_SUB[0] = *(vu16*)0x0689E000;
            mem_setVramHMapping(MEM_VRAM_H_SUB_BG_EXT_PLTT_SLOT_0123);
        }
        _coverGraphicsUploaded = true;
    }
    if (!_showCover || !_selectedFileCover.IsValid() || !_selectedFileCover->IsActualCover())
    {
        // hide cover
        REG_DISPCNT_SUB &= ~(((1 << 3) | (1 << 5)) << 8);
    }
    else
    {
        // display cover
        REG_BG3PA_SUB = 0x100;
        REG_BG3PB_SUB = 0;
        REG_BG3PC_SUB = 0;
        REG_BG3PD_SUB = -0x100;
        REG_BG3X_SUB = (-_coverPosition.x) << 8;
        REG_BG3Y_SUB = (96 + _coverPosition.y - 1) << 8;
        REG_BG3CNT_SUB = 0x0705;
        REG_DISPCNT_SUB |= ((1 << 3) | (1 << 5)) << 8;
        gfx_setSubWindow0(_coverPosition.x, _coverPosition.y, _coverPosition.x + 106, _coverPosition.y + 96);
        REG_WININ_SUB = 0x002A;
        REG_WINOUT_SUB = ~(1 << 3);
    }
    if (!_iconGraphicsUploaded)
    {
        _fileInfoView->UploadIconGraphics();
        _iconGraphicsUploaded = true;
    }
}

void RomBrowserTopScreenView::Draw(GraphicsContext& graphicsContext)
{
    ViewContainer::Draw(graphicsContext);

    if (_batteryFrame == -1)
    {
        return;
    }

    // 1. 딱 필요한 16색(32바이트)만 복사 후 팔레트 할당 (스택 터짐 방지!)
    u16 paddedPal[16] = {0};
    memcpy(paddedPal, batteryPal, 16 * sizeof(u16));
    u32 paletteSlot = graphicsContext.GetPaletteManager().AllocRow(DirectPalette(paddedPal));

    // 2. OAM 1개 할당
    gfx_oam_entry_t* batteryOam = graphicsContext.GetOamManager().AllocOams(1);

    // 3. 현재 프레임에 맞는 VRAM 오프셋 계산 (프레임당 256바이트)
    u32 currentFrameVramOffset = _batteryVramOffset + (_batteryFrame * 256);

    // 4. 화면 우상단(238, 1)에 우선순위 맞춰서 렌더링!
    OamBuilder::OamWithSize<32, 16>(238, 1, currentFrameVramOffset >> 7)
        .WithPalette16(paletteSlot)
        .WithPriority(graphicsContext.GetPriority()) 
        .Build(batteryOam[0]);
}

void RomBrowserTopScreenView::UpdateBatteryLevel()
{
    u32 value = SHARED_BATTERY_LEVEL; 
    unsigned int battery_level = value & BATTERY_LEVEL_MASK;
    bool charger_connected = value & BATTERY_CHARGER_CONNECTED;

    if (charger_connected)
    {
        _batteryFrame = 6;
    }
    else if (isDSiMode())
    {
        int barIndex = battery_level / 4; 
        if (barIndex > 3) barIndex = 3; 
        _batteryFrame = 2 + barIndex;
    }
    else
    {
        _batteryFrame = (battery_level > 3) ? 1 : 0;
    }

    s_lastKnownBatteryFrame = _batteryFrame;
}