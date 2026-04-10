#pragma once
#include "gui/views/View.h"
#include "gui/views/Label2DView.h"
#include "../viewModels/LoadingViewModel.h"
#include "gui/OamManager.h"
#include "gui/OamBuilder.h"
#include "gui/VramContext.h"
#include "libtwl/gfx/gfxPalette.h"

class IFontRepository;
class MaterialColorScheme;

class LoadingView : public View
{
public:
    LoadingView(LoadingViewModel* viewModel, 
                const IFontRepository* fontRepository, 
                const MaterialColorScheme* colorScheme);

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;

    // 추가: View의 순수 가상 함수 구현
    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

private:
    LoadingViewModel* _viewModel;
    Label2DView _loadingLabel;

    u32 _bg1VramOffset = 0;
    u32 _bg2VramOffset = 0;
    
    // 🔥 팔레트 업로드 여부 확인용 플래그
    bool _paletteUploaded = false;
};