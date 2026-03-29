#include "common.h"
#include <string.h>
#include <nds/arm9/cache.h>
#include "fat/File.h"
#include "NdsInternalFileInfo.h"

NdsInternalFileInfo::NdsInternalFileInfo(const FastFileRef& fastFileRef)
{
    const auto file = std::make_unique<File>();
    memset(_gameCode, 0, sizeof(_gameCode));

    file->Open(fastFileRef, FA_READ);

    u32 bannerOffset;
    if (file->Seek(0xC) != FR_OK ||
        !file->ReadExact(_gameCode, 4) ||
        file->Seek(0x68) != FR_OK ||
        !file->ReadExact(&bannerOffset, 4) ||
        bannerOffset == 0 ||
        bannerOffset >= file->GetSize() ||
        file->Seek(bannerOffset) != FR_OK ||
        !file->ReadExact(&_banner, 0x840))
    {
        return;
    }

    if (_banner.header.version >= NDS_BANNER_VERSION_2 &&
        !file->ReadExact(((u8*)&_banner) + 0x840, 0x100))
    {
        return;
    }

    if (_banner.header.version >= NDS_BANNER_VERSION_3 &&
        !file->ReadExact(((u8*)&_banner) + 0x940, 0x100))
    {
        return;
    }

    if (_banner.header.version >= NDS_BANNER_VERSION_103 &&
        !file->ReadExact(((u8*)&_banner) + 0xA40, 0x1980))
    {
        return;
    }

    _hasBanner = true;
    DC_FlushRange(&_banner, sizeof(_banner));
}

const char16_t* NdsInternalFileInfo::GetGameTitle() const
{
    if (!_hasBanner)
        return nullptr;

    if(_banner.header.version < NDS_BANNER_VERSION_3)
        return _banner.title[NDS_BANNER_TITLE_LANGUAGE_ENGLISH];
        
    if(_banner.title[NDS_BANNER_TITLE_LANGUAGE_KOREAN][0] == 0)
        return _banner.title[NDS_BANNER_TITLE_LANGUAGE_ENGLISH];
    else
        return _banner.title[NDS_BANNER_TITLE_LANGUAGE_KOREAN];
}

std::unique_ptr<FileIcon> NdsInternalFileInfo::CreateGameIcon() const
{
    return _hasBanner
        ? std::make_unique<NdsFileIcon>(&_banner)
        : nullptr;
}
