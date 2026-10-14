#pragma once
#include "core/SharedPtr.h"
#include <optional>
#include "services/settings/AppSettings.h"

class SdFolder;
class RomBrowserStateMachine;
class RomBrowserViewModel;
class FileInfo;
class TaskQueueBase;
class ICoverRepository;
class ICheatRepository;
class LanguageRepository;

class IRomBrowserController
{
public:
    virtual ~IRomBrowserController() = 0;

    virtual void NavigateUp() = 0;
    virtual void NavigateToPath(const TCHAR* name) = 0;
    virtual void LaunchFile(const FileInfo& fileInfo) = 0;
    virtual void ShowGameInfo(const FileInfo& fileInfo) = 0;
    virtual void HideGameInfo() = 0;
    virtual void ShowDisplaySettings() = 0;
    virtual void HideDisplaySettings() = 0;

    virtual void Update() = 0;

    virtual const SdFolder& GetSdFolder() const = 0;

    virtual const RomBrowserStateMachine& GetStateMachine() const = 0;

    virtual const SharedPtr<RomBrowserViewModel>& GetRomBrowserViewModel() = 0;

    virtual TaskQueueBase* GetIoTaskQueue() const = 0;
    virtual TaskQueueBase* GetBgTaskQueue() const = 0;
    virtual const ICoverRepository& GetCoverRepository() const = 0;
    virtual const ICheatRepository& GetCheatRepository() const = 0;
    virtual const LanguageRepository& GetLanguageRepository() const = 0;

    virtual const RomBrowserDisplaySettings& GetRomBrowserDisplaySettings() const = 0;

    virtual void SetRomBrowserDisplaySettings(
        const RomBrowserDisplaySettings& romBrowserDisplaySettings) = 0;

    virtual const FileInfo& GetTriggerFileInfo() const = 0;

    virtual const char* GetLanguage() const = 0;

    /// @brief Stores the language in settings.json; the launcher restarts when the
    ///        display settings are closed so the new language pack gets loaded.
    virtual void SetLanguage(const char* language) = 0;

    virtual void ShowLaunchSettings(const FileInfo& fileInfo) = 0;
    virtual void HideLaunchSettings() = 0;

    /// @brief Returns the loader chosen for the launch settings file, or \c std::nullopt for the default loader.
    virtual std::optional<LoaderType> GetGameLoader() const = 0;

    /// @brief Sets the loader for the launch settings file; \c std::nullopt resets it to the default loader.
    virtual void SetGameLoader(std::optional<LoaderType> loader) = 0;
};

inline IRomBrowserController::~IRomBrowserController() { }
