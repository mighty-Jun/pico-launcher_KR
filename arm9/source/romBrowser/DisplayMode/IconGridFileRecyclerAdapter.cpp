#include "common.h"
#include "../FileInfoManager.h"
#include "core/task/TaskQueue.h"
#include "../views/IconGridItemView.h"
#include "../Theme/IRomBrowserViewFactory.h"
#include "IconGridFileRecyclerAdapter.h"

void IconGridFileRecyclerAdapter::GetViewSize(int& width, int& height) const
{
    width = 44;
    height = 44;
}

SharedPtr<View> IconGridFileRecyclerAdapter::CreateView() const
{
    return _romBrowserViewFactory->CreateIconGridItemView();
}

void IconGridFileRecyclerAdapter::BindView(SharedPtr<View> view, int index) const
{
    auto iconGridItemView = static_cast<IconGridItemView*>(view.GetPointer());
    iconGridItemView->SetGraphics(_iconGridItemViewGraphics);
    FileRecyclerAdapter::BindView(view, index);
}

TaskResult<void> IconGridFileRecyclerAdapter::BindView(SharedPtr<View> view, int index,
    const InternalFileInfo* internalFileInfo, const vu8& cancelRequested) const
{
    auto iconGridItemView = static_cast<IconGridItemView*>(view.GetPointer());
    auto icon = internalFileInfo ? internalFileInfo->CreateGameIcon() : nullptr;
    if (!icon)
    {
        const auto& fileInfo = _fileInfoManager->GetItem(index);
        icon = fileInfo.GetFileType()->CreateFileIcon(fileInfo.GetFileName(), _themeFileIconFactory);
    }
    if (icon != nullptr)
    {
        if (cancelRequested)
        {
            icon.reset();
            _fileInfoManager->ReleaseFileInfo(index);
            return TaskResult<void>::Canceled();
        }
        icon->SetAnimFrame(_iconFrameCounter);
        iconGridItemView->SetIcon(std::move(icon));
        iconGridItemView->UploadIconGraphics();
        if (cancelRequested)
        {
            iconGridItemView->SetIcon(nullptr);
            _fileInfoManager->ReleaseFileInfo(index);
            return TaskResult<void>::Canceled();
        }
    }
    return TaskResult<void>::Completed();
}

void IconGridFileRecyclerAdapter::ReleaseView(SharedPtr<View> view, int index) const
{
    LOG_DEBUG("Releasing %d\n", index);
    auto iconGridItemView = static_cast<IconGridItemView*>(view.GetPointer());
    iconGridItemView->SetIcon(nullptr);
    _fileInfoManager->ReleaseFileInfo(index);
}

void IconGridFileRecyclerAdapter::InitVram(const VramContext& vramContext)
{
    _iconGridItemViewGraphics = _romBrowserViewFactory->UploadIconGridItemViewGraphics(vramContext);
}
