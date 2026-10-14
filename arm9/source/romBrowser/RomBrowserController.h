#pragma once
#include <memory>
#include "core/SharedPtr.h"
#include "SdFolder.h"
#include "viewModels/RomBrowserViewModel.h"
#include "RomBrowserStateMachine.h"
#include "core/task/TaskQueue.h"
#include "IRomBrowserController.h"
#include "CoverRepository.h"
#include "FileType/ExtensionFileTypeProvider.h"
#include "services/settings/IAppSettingsService.h"
#include "services/settings/IGameSettingsService.h"
#include "cheats/ICheatRepository.h"

class RomBrowserController : public IRomBrowserController
{
public:
    RomBrowserController(IAppSettingsService* appSettingsService, IGameSettingsService* gameSettingsService,
        TaskQueueBase* ioTaskQueue, TaskQueueBase* bgTaskQueue);

    void NavigateUp() override
    {
        NavigateToPath("..");
    }

    void NavigateToPath(const TCHAR* name) override;
    void LaunchFile(const FileInfo& fileInfo) override;
    void ShowGameInfo(const FileInfo& fileInfo) override;
    void HideGameInfo() override;
    void ShowDisplaySettings() override;
    void HideDisplaySettings() override;

    void Update() override;

    const SdFolder& GetSdFolder() const override { return *_sdFolder; }

    const RomBrowserStateMachine& GetStateMachine() const override { return _stateMachine; }

    const SharedPtr<RomBrowserViewModel>& GetRomBrowserViewModel() override { return _romBrowserViewModel; }

    TaskQueueBase* GetIoTaskQueue() const override { return _ioTaskQueue; }
    TaskQueueBase* GetBgTaskQueue() const override { return _bgTaskQueue; }
    const ICoverRepository& GetCoverRepository() const override { return *_coverRepository; }
    const ICheatRepository& GetCheatRepository() const override { return *_cheatRepository; }

    void SetRomBrowserDisplaySettings(const RomBrowserDisplaySettings& romBrowserDisplaySettings) override;

    void SaveSettingsNow() override;

    void RequestThemeReload() override { _themeReloadRequested = true; }

    bool ConsumeThemeReloadRequest()
    {
        const bool requested = _themeReloadRequested;
        _themeReloadRequested = false;
        return requested;
    }

    const RomBrowserDisplaySettings& GetRomBrowserDisplaySettings() const override
    {
        return _appSettingsService->GetAppSettings().romBrowserDisplaySettings;
    }

    virtual const FileInfo& GetTriggerFileInfo() const override { return _triggerFileInfo; }

    void ShowLaunchSettings(const FileInfo& fileInfo) override;
    void HideLaunchSettings() override;
    std::optional<LoaderType> GetGameLoader() const override;
    void SetGameLoader(std::optional<LoaderType> loader) override;

    /// @brief Returns whether the file that is being launched goes through nds-bootstrap.
    bool IsLaunchingWithNdsBootstrap() const { return _launchWithNdsBootstrap; }

private:
    IAppSettingsService* _appSettingsService;
    IGameSettingsService* _gameSettingsService;
    TaskQueueBase* _ioTaskQueue;
    TaskQueueBase* _bgTaskQueue;

    std::unique_ptr<SdFolder> _sdFolder;
    SharedPtr<RomBrowserViewModel> _romBrowserViewModel;
    std::unique_ptr<SdFolder> _newSdFolder;
    RomBrowserStateMachine _stateMachine;
    TCHAR _navigatePath[256];
    TCHAR* _navigateFileName;
    TCHAR _currentFolderPath[256] = "";
    FileInfo _triggerFileInfo;
    QueueTask<void> _navigateTask;
    bool _saveSettingsPending = false;
    bool _saveGameSettingsPending = false;
    bool _themeReloadRequested = false;
    bool _launchWithNdsBootstrap = false;
    std::unique_ptr<CoverRepository> _coverRepository;
    ExtensionFileTypeProvider _fileTypeProvider;
    std::unique_ptr<ICheatRepository> _cheatRepository;

    void HandleTrigger();
    void HandleNavigateTrigger();
    void HandleFolderLoadDoneTrigger();
    void HandleLaunchTrigger();
    void HandleChangeDisplayModeTrigger();
    void UpdateLastUsedFilepath();
    void GetTriggerFilePath(TCHAR* path, u32 length) const;
    bool ShouldLaunchWithNdsBootstrap() const;
    void SetPicoLoaderParams() const;
    void LaunchWithNdsBootstrap() const;
    void LoadCheats() const;
    void SaveSettingsAsync();
};
