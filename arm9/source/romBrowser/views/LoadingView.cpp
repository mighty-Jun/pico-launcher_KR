#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "gui/OamBuilder.h"
#include "gui/VramContext.h"
#include "gui/palette/DirectPalette.h"
#include "services/Language/LanguagePack.h"
#include "themes/IFontRepository.h"
#include "themes/material/MaterialColorScheme.h"
#include "loadingBg_1.h"
#include "loadingBg_2.h"
#include "LoadingView.h"

#define BACKGROUND_X    64
#define BACKGROUND_Y    64

LoadingView::LoadingView(const IFontRepository* fontRepository, const MaterialColorScheme* colorScheme,
    const LanguagePack& languagePack)
    : _loadingLabel(Label2DView::CreateShared(128, 16, 25, fontRepository->GetFont(FontType::Medium11)))
{
    _loadingLabel->SetText(languagePack.loading_message.GetString());
    _loadingLabel->SetHorizontalAlignment(Alignment::Start);
    _loadingLabel->SetPosition(104, 96 - 8);
    _loadingLabel->SetBackgroundColor(colorScheme->GetColor(md::sys::color::primary));
    _loadingLabel->SetForegroundColor(colorScheme->onSurface);
    _loadingLabel->SetParent(this);
}

void LoadingView::InitVram(const VramContext& vramContext)
{
    _loadingLabel->InitVram(vramContext);

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
    _loadingLabel->Update();
}

void LoadingView::Draw(GraphicsContext& graphicsContext)
{
    auto& paletteManager = graphicsContext.GetPaletteManager();
    u32 bg1PaletteRow = paletteManager.AllocRow(DirectPalette((const u16*)loadingBg_1Pal),
        BACKGROUND_Y, BACKGROUND_Y + 64);
    u32 bg2PaletteRow = paletteManager.AllocRow(DirectPalette((const u16*)loadingBg_2Pal),
        BACKGROUND_Y, BACKGROUND_Y + 64);

    gfx_oam_entry_t* oams = graphicsContext.GetOamManager().AllocOams(2);
    int priority = graphicsContext.GetPriority();

    // the 128x64 background is made of two 64x64 halves
    OamBuilder::OamWithSize<64, 64>(BACKGROUND_X, BACKGROUND_Y, _bg1VramOffset >> 7)
        .WithPalette16(bg1PaletteRow)
        .WithPriority(priority)
        .Build(oams[0]);
    OamBuilder::OamWithSize<64, 64>(BACKGROUND_X + 64, BACKGROUND_Y, _bg2VramOffset >> 7)
        .WithPalette16(bg2PaletteRow)
        .WithPriority(priority)
        .Build(oams[1]);

    _loadingLabel->Draw(graphicsContext);
}

void LoadingView::VBlank()
{
    _loadingLabel->VBlank();
}
