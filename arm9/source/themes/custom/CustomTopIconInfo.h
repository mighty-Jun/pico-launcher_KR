#pragma once
#include "core/math/Point.h"
#include "core/math/Rgb.h"

class CustomTopIconInfo
{
public:
    CustomTopIconInfo(const Point& position, const Rgb8& blendColor)
        : _position(position), _blendColor(blendColor) { }

    const Point& GetPosition() const { return _position; }
    const Rgb8& GetBlendColor() const { return _blendColor; }

private:
    Point _position;
    Rgb8 _blendColor;
};
