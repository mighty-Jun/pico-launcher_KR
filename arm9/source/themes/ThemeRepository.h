#pragma once
#include <memory>
#include "romBrowser/SdFolder.h"
#include "themes/ThemeInfo.h"
#include "themes/ThemeInfoFactory.h"

class ThemeRepository
{
public:
    void Initialize();

    u32 GetThemeCount() const;
    std::unique_ptr<ThemeInfo> LoadThemeInfo(u32 themeIndex) const;

private:
    ThemeInfoFactory _themeInfoFactory;
    std::unique_ptr<SdFolder> _themesFolder;
    u32 _numberOfThemes = 0;
    std::unique_ptr<const FileInfo*[]> _themeFolders;
};
