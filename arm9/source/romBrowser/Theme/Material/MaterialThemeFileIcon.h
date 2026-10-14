#pragma once
#include "MaterialFileIcon.h"
#include "themeIconLarge.h"
#include "themes/material/MaterialColorScheme.h"

class MaterialThemeFileIcon : public MaterialFileIcon
{
public:
    MaterialThemeFileIcon(const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
        : MaterialFileIcon("", materialColorScheme, fontRepository) { }

protected:
    const void* GetIconTiles() const override
    {
        return themeIconLargeTiles;
    }

    Rgb<8, 8, 8> GetIconColor() const override
    {
        return _materialColorScheme->tertiary;
    }

    Rgb<8, 8, 8> GetTextColor() const override
    {
        return _materialColorScheme->onTertiary;
    }

    int GetTextYOffset() const override { return 0; }
};
