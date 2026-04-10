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
    _loadingLabel.SetText(u"로딩중..");
    _loadingLabel.SetHorizontalAlignment(Alignment::Start);
    // 하단 화면 중앙에 배치
    _loadingLabel.SetPosition(128, 96 - 8); 
    
    // 배경색과 글자색 테마 적용
    _loadingLabel.SetBackgroundColor(colorScheme->GetColor(md::sys::color::primary));
    _loadingLabel.SetForegroundColor(colorScheme->onSurface);

    // AddChildTail(&_loadingLabel); <-- 이 줄을 지우고 아래 줄로 변경하세요.
    _loadingLabel.SetParent(this);
}

void LoadingView::InitVram(const VramContext& vramContext)
{
    _loadingLabel.InitVram(vramContext);

    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        // 타일 데이터 업로드 (배터리 코드와 동일)
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
        .WithPalette16(14) // 🔥 왼쪽은 14번 팔레트를 사용합니다.
        .WithPriority(prio)
        .Build(oams[0]);

    // 오른쪽 조각 (X: 128, Y: 64)
    OamBuilder::OamWithSize<64, 64>(
            128, 64, 
            _bg2VramOffset >> 7)
        .WithPalette16(15) // 🔥 오른쪽은 자기 전용 15번 팔레트를 사용합니다.
        .WithPriority(prio)
        .Build(oams[1]);

    _loadingLabel.Draw(graphicsContext);
}

void LoadingView::VBlank()
{
    if (!_paletteUploaded)
    {
        u16* mainObjPalette = (u16*)GFX_PLTT_OBJ_MAIN;
        
        // 🔥 안전한 복사: DMA 덮어쓰기 버그(상단 화면 침범)를 원천 차단하기 위해
        // 배열 길이 변수(PalLen)를 믿지 않고, 딱 16개의 색상만 수동으로 정확히 꽂아 넣습니다.
        
        // 왼쪽 팝업 팔레트는 14번 슬롯에 복사
        for (int i = 0; i < 16; i++) {
            mainObjPalette[(14 * 16) + i] = loadingBg_1Pal[i];
        }
        
        // 오른쪽 팝업 팔레트는 15번 슬롯에 복사 (오른쪽이 회색으로 나오는 문제 완벽 해결!)
        for (int i = 0; i < 16; i++) {
            mainObjPalette[(15 * 16) + i] = loadingBg_2Pal[i];
        }

        _paletteUploaded = true;
    }

    _loadingLabel.VBlank();
}