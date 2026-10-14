#include "common.h"
#include "romBrowser/FileType/BmpFileIcon.h"
#include "romBrowser/Theme/IRomBrowserViewFactory.h"
#include "romBrowser/Theme/IThemeFileIconFactory.h"
#include "settings/SettingsController.h"
#include "settings/ThemeInfoManager.h"
#include "settings/viewModels/ThemeListItemViewModel.h"
#include "themes/ThemeInfo.h"
#include "ThemeAdapter.h"

ThemeAdapter::ThemeAdapter(ISettingsController* settingsController, const IRomBrowserViewFactory* romBrowserViewFactory,
    const IThemeFileIconFactory* themeFileIconFactory, VBlankTextureLoader* vblankTextureLoader)
    : _settingsController(settingsController), _romBrowserViewFactory(romBrowserViewFactory)
    , _themeFileIconFactory(themeFileIconFactory), _vblankTextureLoader(vblankTextureLoader) { }

u32 ThemeAdapter::GetItemCount() const
{
    return _settingsController->GetThemeRepository().GetThemeCount();
}

void ThemeAdapter::GetViewSize(int& width, int& height) const
{
    width = 203;
    height = 44;
}

SharedPtr<View> ThemeAdapter::CreateView() const
{
    return _romBrowserViewFactory->CreateBannerListItemView(
        std::make_unique<ThemeListItemViewModel>(_settingsController), _vblankTextureLoader);
}

void ThemeAdapter::BindView(SharedPtr<View> view, int index) const
{
    auto listItemView = static_cast<BannerListItemView*>(view.GetPointer());
    listItemView->SetGraphics(_listItemVramToken);
    LOG_DEBUG("Binding %d\n", index);
    auto queueTask = _settingsController->GetIoTaskQueue()->Enqueue([=, this] (const vu8& cancelRequested)
    {
        if (cancelRequested)
        {
            LOG_DEBUG("Task to load %d was canceled\n", index);
            return TaskResult<void>::Canceled();
        }

        LOG_DEBUG("Started task to load %d\n", index);
        auto& themeInfoManager = _settingsController->GetThemeInfoManager();
        themeInfoManager.LoadThemeInfo(index);
        if (cancelRequested)
        {
            return TaskResult<void>::Canceled();
        }

        auto extraThemeInfo = themeInfoManager.GetExtraThemeInfo(index);
        auto listItemView = static_cast<BannerListItemView*>(view.GetPointer());
        listItemView->GetViewModel().SetIndex(index);
        if (extraThemeInfo)
        {
            listItemView->SetFirstLineAsync(nullptr, extraThemeInfo->themeInfo->GetName(), false);
            listItemView->SetSecondLineAsync(nullptr, extraThemeInfo->themeInfo->GetAuthor());
            listItemView->SetThirdLineAsync(nullptr, extraThemeInfo->themeInfo->GetDescription());
            if (extraThemeInfo->iconData)
            {
                listItemView->SetIcon(std::make_unique<BmpFileIcon>(extraThemeInfo->iconData));
            }
            else
            {
                listItemView->SetIcon(_themeFileIconFactory->CreateThemeFileIcon());
            }
            listItemView->UploadIconGraphics();
            static_cast<ThemeListItemViewModel&>(listItemView->GetViewModel()).SetExtraThemeInfo(extraThemeInfo);
        }

        return TaskResult<void>::Completed();
    });

    listItemView->GetViewModel().SetQueueTask(std::move(queueTask));
}

void ThemeAdapter::ReleaseView(SharedPtr<View> view, int index) const
{
    auto listItemView = static_cast<BannerListItemView*>(view.GetPointer());
    listItemView->SetIcon(nullptr);
    listItemView->GetViewModel().SetIndex(-1);
    listItemView->GetViewModel().CancelQueueTask();
}

void ThemeAdapter::InitVram(const VramContext& vramContext)
{
    _listItemVramToken = _romBrowserViewFactory->UploadBannerListItemViewGraphics(vramContext);
}
