#include "common.h"
#include "LoadingView.h"
#include "gui/GraphicsContext.h"
#include "themes/IFontRepository.h"
#include "themes/material/MaterialColorScheme.h"

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

void LoadingView::Update()
{
    _loadingLabel.Update();
}

void LoadingView::Draw(GraphicsContext& graphicsContext)
{
    _loadingLabel.Draw(graphicsContext);
}

void LoadingView::VBlank()
{
    _loadingLabel.VBlank();
}