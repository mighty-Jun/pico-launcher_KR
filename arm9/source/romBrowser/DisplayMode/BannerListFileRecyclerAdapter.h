#pragma once
#include "../FileRecyclerAdapter.h"
#include "../views/BannerListItemView.h"

class TaskQueueBase;
class IRomBrowserViewFactory;

class BannerListFileRecyclerAdapter : public FileRecyclerAdapter
{
public:
    BannerListFileRecyclerAdapter(FileInfoManager* fileInfoManager,
        TaskQueueBase* taskQueue, const IThemeFileIconFactory* themeFileIconFactory,
        const IRomBrowserViewFactory* romBrowserViewFactory,
        VBlankTextureLoader* vblankTextureLoader)
        : FileRecyclerAdapter(fileInfoManager, taskQueue, themeFileIconFactory)
        , _romBrowserViewFactory(romBrowserViewFactory)
        , _vblankTextureLoader(vblankTextureLoader) { }

    void GetViewSize(int& width, int& height) const override;
    SharedPtr<View> CreateView() const override;
    void BindView(SharedPtr<View> view, int index) const override;
    void ReleaseView(SharedPtr<View> view, int index) const override;

    void InitVram(const VramContext& vramContext) override;

private:
    const IRomBrowserViewFactory* _romBrowserViewFactory;
    BannerListItemView::VramToken _bannerListItemViewGraphics;
    VBlankTextureLoader* _vblankTextureLoader;

    TaskResult<void> BindView(SharedPtr<View> view, int index,
        const InternalFileInfo* internalFileInfo, const vu8& cancelRequested) const override;
};
