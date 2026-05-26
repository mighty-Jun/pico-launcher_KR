#pragma once
#include "IFontRepository.h"

class DefaultFontRepository : public IFontRepository
{
public:
    const nft3_header_t* GetFont(FontType fontType) const override;
};
