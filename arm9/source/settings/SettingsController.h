#pragma once
#include "ISettingsController.h"
#include "ThemeInfoManager.h"
#include "themes/ThemeRepository.h"

class IAppSettingsService;

class SettingsController : public ISettingsController
{
public:
    SettingsController(IAppSettingsService* appSettingsService, TaskQueueBase* ioTaskQueue);

    void Initialize() override;
    void NavigateUp() override;
    void SelectTheme(const char* themeFolderName) override;

    ThemeInfoManager& GetThemeInfoManager() const override { return *_themeInfoManager; }
    const ThemeRepository& GetThemeRepository() const override { return _themeRepository; }
    TaskQueueBase* GetIoTaskQueue() const override { return _ioTaskQueue; }

private:
    IAppSettingsService* _appSettingsService;
    TaskQueueBase* _ioTaskQueue;
    ThemeRepository _themeRepository;
    std::unique_ptr<ThemeInfoManager> _themeInfoManager;
};
