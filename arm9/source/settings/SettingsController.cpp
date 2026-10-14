#include "common.h"
#include "App.h"
#include "SettingsController.h"

SettingsController::SettingsController(IAppSettingsService* appSettingsService, TaskQueueBase* ioTaskQueue)
    : _appSettingsService(appSettingsService), _ioTaskQueue(ioTaskQueue) { }

void SettingsController::Initialize()
{
    _themeRepository.Initialize();
    _themeInfoManager = std::make_unique<ThemeInfoManager>(_themeRepository);
}

void SettingsController::NavigateUp()
{
    gProcessManager.Goto<App>();
}

void SettingsController::SelectTheme(const char* themeFolderName)
{
    _appSettingsService->GetAppSettings().theme = themeFolderName;
    _ioTaskQueue->Enqueue([this] (const vu8& cancelRequested)
    {
        _appSettingsService->Save();
        return TaskResult<void>::Completed();
    });
    gProcessManager.Goto<App>();
}
