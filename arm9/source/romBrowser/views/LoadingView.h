#pragma once
#include "gui/views/View.h"
#include "gui/views/Label2DView.h"
#include "../viewModels/LoadingViewModel.h"

class IFontRepository;
class MaterialColorScheme;

class LoadingView : public View
{
public:
    LoadingView(LoadingViewModel* viewModel, 
                const IFontRepository* fontRepository, 
                const MaterialColorScheme* colorScheme);

    void InitVram(const VramContext& vramContext) override 
    {
        _loadingLabel.InitVram(vramContext);
    }
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
};