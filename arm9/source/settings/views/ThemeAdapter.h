#pragma once
#include "gui/views/RecyclerAdapter.h"
#include "romBrowser/views/BannerListItemView.h"

class IFontRepository;
class IRomBrowserViewFactory;
class ISettingsController;
class IThemeFileIconFactory;
class MaterialColorScheme;
class VBlankTextureLoader;
class VramContext;

class ThemeAdapter : public RecyclerAdapter
{
public:
    ThemeAdapter(ISettingsController* settingsController, const IRomBrowserViewFactory* romBrowserViewFactory,
        const IThemeFileIconFactory* themeFileIconFactory, VBlankTextureLoader* vblankTextureLoader);

    u32 GetItemCount() const override;
    void GetViewSize(int& width, int& height) const override;
    SharedPtr<View> CreateView() const override;
    void BindView(SharedPtr<View> view, int index) const override;
    void ReleaseView(SharedPtr<View> view, int index) const override;

    void InitVram(const VramContext& vramContext);

private:
    ISettingsController* _settingsController;
    const IRomBrowserViewFactory* _romBrowserViewFactory;
    const IThemeFileIconFactory* _themeFileIconFactory;
    VBlankTextureLoader* _vblankTextureLoader;
    BannerListItemView::VramToken _listItemVramToken;
};
