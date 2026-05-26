#pragma once
#include "gui/font/nitroFont3.h"
#include "FontType.h"

class IFontRepository
{
public:
    virtual ~IFontRepository() = 0;

    virtual const nft3_header_t* GetFont(FontType fontType) const = 0;
};

inline IFontRepository::~IFontRepository() { }
