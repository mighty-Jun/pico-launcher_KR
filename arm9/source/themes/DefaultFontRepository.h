#pragma once
#include "IFontRepository.h"

class DefaultFontRepository : public IFontRepository
{
public:
    const nft2_header_t* GetFont(FontType fontType) const override;
    static void SetFont(FontType fontType, const nft2_header_t* fontPtr);
};
