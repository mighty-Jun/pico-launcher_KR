#pragma once
#include "core/task/TaskQueue.h"

class IRomBrowserItemViewModel
{
public:
    virtual ~IRomBrowserItemViewModel() = default;

    virtual void Activate() = 0;
    virtual void ShowGameInfo() = 0;

    /// @brief Opens the per-game launch settings; items that have none ignore it.
    virtual void ShowLaunchSettings() { }
    virtual void SetIndex(int index) = 0;
    virtual void SetQueueTask(QueueTask<void> queueTask) = 0;
    virtual void CancelQueueTask() = 0;
    virtual void DisposeQueueTaskWhenComplete() = 0;

protected:
    IRomBrowserItemViewModel() = default;
};
