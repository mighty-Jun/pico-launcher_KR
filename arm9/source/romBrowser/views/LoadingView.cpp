#include "common.h"
#include "LoadingView.h"
#include "gui/GraphicsContext.h"
#include "themes/IFontRepository.h"
#include "themes/material/MaterialColorScheme.h"

#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxBackground.h>

#include "loadingBg_1.h"
#include "loadingBg_2.h"

LoadingView::LoadingView(LoadingViewModel* viewModel, 
                         const IFontRepository* fontRepository, 
                         const MaterialColorScheme* colorScheme)
    : _viewModel(viewModel)
    , _loadingLabel(128, 16, 25, fontRepository->GetFont(FontType::Medium11))
{
    _loadingLabel.SetText(u"로딩 중..");
    _loadingLabel.SetHorizontalAlignment(Alignment::Start);
    _loadingLabel.SetPosition(104, 96 - 8); 
    
    _loadingLabel.SetBackgroundColor(colorScheme->GetColor(md::sys::color::primary));
    _loadingLabel.SetForegroundColor(colorScheme->onSurface);

    _loadingLabel.SetParent(this);
}

void LoadingView::InitVram(const VramContext& vramContext)
{
    _loadingLabel.InitVram(vramContext);

    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        _bg1VramOffset = objVramManager->Alloc(loadingBg_1TilesLen);
        dma_ntrCopy32(3, loadingBg_1Tiles, objVramManager->GetVramAddress(_bg1VramOffset), loadingBg_1TilesLen);

        _bg2VramOffset = objVramManager->Alloc(loadingBg_2TilesLen);
        dma_ntrCopy32(3, loadingBg_2Tiles, objVramManager->GetVramAddress(_bg2VramOffset), loadingBg_2TilesLen);
    }
}

void LoadingView::Update()
{
    _loadingLabel.Update();
}

void LoadingView::Draw(GraphicsContext& graphicsContext)
{
    gfx_oam_entry_t* oams = graphicsContext.GetOamManager().AllocOams(2);
    int prio = graphicsContext.GetPriority(); 

    // 왼쪽 조각 (X: 64, Y: 64)
    OamBuilder::OamWithSize<64, 64>(
            64, 64, 
            _bg1VramOffset >> 7)
        .WithPalette16(14)
        .WithPriority(prio)
        .Build(oams[0]);

    // 오른쪽 조각 (X: 128, Y: 64)
    OamBuilder::OamWithSize<64, 64>(
            128, 64, 
            _bg2VramOffset >> 7)
        .WithPalette16(15)
        .WithPriority(prio)
        .Build(oams[1]);

    _loadingLabel.Draw(graphicsContext);
}

void LoadingView::VBlank()
{
    if (!_paletteUploaded)
    {
        u16* mainObjPalette = (u16*)GFX_PLTT_OBJ_MAIN;
        
        for (int i = 0; i < 16; i++) {
            mainObjPalette[(14 * 16) + i] = loadingBg_1Pal[i];
        }
        
        for (int i = 0; i < 16; i++) {
            mainObjPalette[(15 * 16) + i] = loadingBg_2Pal[i];
        }

        _paletteUploaded = true;
    }

    _loadingLabel.VBlank();
}