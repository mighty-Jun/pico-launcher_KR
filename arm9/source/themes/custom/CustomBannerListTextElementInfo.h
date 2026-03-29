#pragma once
#include "core/math/Point.h"
#include "core/math/Rgb.h"

class CustomBannerListTextElementInfo
{
public:
    CustomBannerListTextElementInfo(const Rgb8& textColor)
        : _textColor(textColor) { }

    const Rgb8& GetTextColor() const { return _textColor; }

private:
    Rgb8 _textColor;
};
