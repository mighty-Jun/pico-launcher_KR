#pragma once
#include "../InternalFileInfo.h"
#include "ndsBanner.h"
#include "NdsFileIcon.h"
#include "fat/FastFileRef.h"

/// @brief Internal file info for nds roms.
class alignas(32) NdsInternalFileInfo : public InternalFileInfo
{
public:
    explicit NdsInternalFileInfo(const FastFileRef& fastFileRef);

    constexpr const char* GetGameCode() const override { return _gameCode; }
    const char16_t* GetGameTitle() const override;
    std::unique_ptr<FileIcon> CreateGameIcon() const override;
    const nds_banner_t& GetBanner() const { return _banner; }

private:
    nds_banner_t _banner alignas(32);
    bool _hasBanner = false;
    char _gameCode[5];
};