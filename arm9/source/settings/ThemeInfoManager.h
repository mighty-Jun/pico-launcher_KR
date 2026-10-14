#pragma once
#include "common.h"
#include <memory>
#include "romBrowser/FileType/BmpFileIconData.h"
#include "themes/ThemeRepository.h"
#include "core/AtomicSharedPtr.h"

class ThemeInfoManager
{
public:
    struct alignas(32) ExtraThemeInfo
    {
        u16 previewImage[256 * 192];
        std::unique_ptr<ThemeInfo> themeInfo;
        SharedPtr<BmpFileIconData> iconData;
    };

    ThemeInfoManager(const ThemeRepository& themeRepository);

    SharedPtr<ExtraThemeInfo> GetExtraThemeInfo(int index) const
    {
        return _extraThemeInfo[index].Lock();
    }

    void LoadThemeInfo(int index);
    void ReleaseThemeInfo(int index);

    u32 GetItemCount() const { return _themeRepository.GetThemeCount(); }

private:
    const ThemeRepository& _themeRepository;
    std::unique_ptr<AtomicSharedPtr<ExtraThemeInfo>[]> _extraThemeInfo;

    void LoadThemeIcon(SharedPtr<ExtraThemeInfo>& extraThemeInfo);
    void LoadThemePreviewImage(SharedPtr<ExtraThemeInfo>& extraThemeInfo);
};