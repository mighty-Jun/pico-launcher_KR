#include "common.h"
#include "romBrowser/FileType/NullFileTypeProvider.h"
#include "romBrowser/SdFolderFactory.h"
#include "ThemeRepository.h"

void ThemeRepository::Initialize()
{
    NullFileTypeProvider fileTypeProvider;
    _themesFolder = SdFolderFactory(&fileTypeProvider).CreateFromPath("/_pico/themes");
    auto filterSortParams = SdFolderFilterSortParams(SdFolderSortType::Name, SdFolderSortDirection::Ascending, false);
    _themeFolders = _themesFolder->FilterAndSort(filterSortParams, _numberOfThemes);
}

u32 ThemeRepository::GetThemeCount() const
{
    return _numberOfThemes;
}

std::unique_ptr<ThemeInfo> ThemeRepository::LoadThemeInfo(u32 themeIndex) const
{
    if (themeIndex >= _numberOfThemes)
    {
        return nullptr;
    }

    return _themeInfoFactory.CreateFromThemeFolder(_themeFolders[themeIndex]->GetFileName());
}
