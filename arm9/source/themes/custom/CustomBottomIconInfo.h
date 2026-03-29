#pragma once
#include "core/math/Point.h"
#include "core/math/Rgb.h"

class CustomBottomIconInfo
{
public:
    CustomBottomIconInfo(const Rgb8& blendColor)
        : _blendColor(blendColor) { }

    const Rgb8& GetBlendColor() const { return _blendColor; }

private:
    Rgb8 _blendColor;
};
