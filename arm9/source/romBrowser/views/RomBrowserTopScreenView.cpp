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

RomBrowserTopScreenView::RomBrowserTopScreenView(
    const SharedPtr<RomBrowserViewModel>& viewModel,
    const RomBrowserDisplayMode* displayMode,
    const IThemeFileIconFactory* themeFileIconFactory,
    const IRomBrowserViewFactory* romBrowserViewFactory)
    : _viewModel(viewModel)
    , _themeFileIconFactory(themeFileIconFactory)
    , _fileInfoView(romBrowserViewFactory->CreateFileInfoView())
    , _showCover(displayMode->ShowCoverOnTopScreen())
{
    AddChildTail(_fileInfoView.get());
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
    ViewContainer::Update();
}

void RomBrowserTopScreenView::VBlank()
{
    ViewContainer::VBlank();

    if (!_batteryPaletteUploaded)
    {
        // 서브 화면 OBJ 팔레트의 15번 슬롯(가장 마지막 슬롯)에 16색(32바이트) 팔레트를 복사합니다.
        u16* subObjPalette = (u16*)0x068A0000;
        dma_ntrCopy32(3, batteryPal, subObjPalette + (15 * 16), batteryPalLen);
        _batteryPaletteUploaded = true;
    }

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
        REG_BG3X_SUB = -75 << 8;
        REG_BG3Y_SUB = 113 << 8;
        REG_BG3CNT_SUB = 0x0705;
        REG_DISPCNT_SUB |= ((1 << 3) | (1 << 5)) << 8;
        gfx_setSubWindow0(75, 18, 75 + 106, 18 + 96);
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
    // 자식 뷰(FileInfo 등) 먼저 그리기
    ViewContainer::Draw(graphicsContext);

    // OAM 매니저를 통해 스프라이트 1개 할당
    gfx_oam_entry_t* batteryOam = graphicsContext.GetOamManager().AllocOams(1);

    // OamBuilder를 이용해 중앙에 렌더링
    // 💡주의: <32, 16>은 battery.png의 해상도입니다. 실제 해상도(예: 64, 32 등)에 맞춰 숫자를 변경해 주세요.
    OamBuilder::OamWithSize<32, 16>(
            112, 88, // X: 112, Y: 88 (화면 정중앙 근처)
            _batteryVramOffset >> 7)
        .WithPalette16(15) // 15번 팔레트 슬롯 사용
        .WithPriority(0)   // 최상단 표시 우선순위
        .Build(batteryOam[0]);
}