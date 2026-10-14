#pragma once
class TaskQueueBase;
class ThemeInfoManager;
class ThemeRepository;

class ISettingsController
{
public:
    virtual ~ISettingsController() = default;

    virtual void Initialize() = 0;
    virtual void NavigateUp() = 0;
    virtual void SelectTheme(const char* themeFolderName) = 0;

    virtual ThemeInfoManager& GetThemeInfoManager() const = 0;
    virtual const ThemeRepository& GetThemeRepository() const = 0;
    virtual TaskQueueBase* GetIoTaskQueue() const = 0;
};
