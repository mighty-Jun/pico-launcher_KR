#pragma once
#include "MaterialIconGridItemView.h"
#include "MaterialBannerListItemView.h"
#include "MaterialFileInfoCardView.h"
#include "../IRomBrowserViewFactory.h"
#include "romBrowser/viewModels/RomBrowserViewModel.h"
#include "CarouselRecyclerView.h"
#include "MaterialCoverFlowFileRecyclerAdapter.h"
#include "MaterialAppBarView.h"

class MaterialColorScheme;
class IFontRepository;

class MaterialRomBrowserViewFactory : public IRomBrowserViewFactory
{
public:
    MaterialRomBrowserViewFactory(const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository)
        : _materialColorScheme(materialColorScheme), _fontRepository(fontRepository) { }

    SharedPtr<IconGridItemView> CreateIconGridItemView() const override
    {
        return SharedPtr<MaterialIconGridItemView>::MakeShared(_materialColorScheme);
    }

    IconGridItemView::VramToken UploadIconGridItemViewGraphics(
        const VramContext& vramContext) const override
    {
        return MaterialIconGridItemView::UploadGraphics(vramContext);
    }

    SharedPtr<BannerListItemView> CreateBannerListItemView(VBlankTextureLoader* vblankTextureLoader) const override
    {
        return SharedPtr<MaterialBannerListItemView>::MakeShared(_materialColorScheme, _fontRepository);
    }

    BannerListItemView::VramToken UploadBannerListItemViewGraphics(
        const VramContext& vramContext) const override
    {
        return MaterialBannerListItemView::UploadGraphics(vramContext);
    }

    std::unique_ptr<AppBarView> CreateAppBarView(int x, int y, AppBarView::Orientation orientation,
        int startButtonCount, int endButtonCount) const override
    {
        return std::make_unique<MaterialAppBarView>(x, y, orientation, startButtonCount, endButtonCount, _materialColorScheme);
    }

    std::unique_ptr<BannerView> CreateFileInfoView() const override
    {
        return std::make_unique<MaterialFileInfoCardView>(_materialColorScheme, _fontRepository);
    }

    SharedPtr<RecyclerViewBase> CreateCoverFlowRecyclerView() const override
    {
        return CarouselRecyclerView::CreateShared(_materialColorScheme);
    }

    SharedPtr<FileRecyclerAdapter> CreateCoverFlowRecyclerAdapter(
        RomBrowserViewModel* viewModel, const IThemeFileIconFactory* themeFileIconFactory,
        VBlankTextureLoader* vblankTextureLoader) const override
    {
        return SharedPtr<MaterialCoverFlowFileRecyclerAdapter>::MakeShared(
            &viewModel->GetFileInfoManager(), viewModel->GetIoTaskQueue(),
            themeFileIconFactory, this, vblankTextureLoader, &viewModel->GetCoverRepository());
    }

    Point GetTopCoverPosition() const override
    {
        return Point(75, 18);
    }

private:
    const MaterialColorScheme* _materialColorScheme;
    const IFontRepository* _fontRepository;
};
