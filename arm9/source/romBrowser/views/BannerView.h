#pragma once
#include "gui/views/ViewContainer.h"
#include "../FileType/FileIcon.h"

class TaskQueueBase;

class BannerView : public ViewContainer
{
public:
    void InitVram(const VramContext& vramContext) override;

    void SetFileName(const TCHAR* fileName, bool useAsTitle)
    {
        SetFileNameAsync(nullptr, fileName, useAsTitle);
    }

    virtual void SetFileNameAsync(TaskQueueBase* taskQueue, const TCHAR* fileName, bool useAsTitle);

    void SetGameTitle(const char16_t* gameTitle)
    {
        SetGameTitleAsync(nullptr, gameTitle);
    }

    void SetGameTitleAsync(TaskQueueBase* taskQueue, const char16_t* gameTitle);

    void SetIcon(std::unique_ptr<FileIcon> icon)
    {
        _icon = std::move(icon);
        if (_icon)
        {
            _icon->SetVramAddress(_iconVram, _iconVramOffset);
        }
    }

    void UploadIconGraphics() const
    {
        if (_icon)
        {
            _icon->UploadGraphics();
        }
    }

protected:
    std::unique_ptr<FileIcon> _icon = nullptr;
    vu16* _iconVram;
    u32 _iconVramOffset;
    u32 _lines;

    virtual void SetFirstLineAsync(TaskQueueBase* taskQueue, const char* firstLine, bool ellipsis) = 0;
    virtual void SetFirstLineAsync(TaskQueueBase* taskQueue, const char16_t* firstLine, u32 length, bool ellipsis) = 0;
    virtual void SetSecondLineAsync(TaskQueueBase* taskQueue, const char16_t* secondLine, u32 length) = 0;
    virtual void SetThirdLineAsync(TaskQueueBase* taskQueue, const char16_t* thirdLine, u32 length) = 0;
};
