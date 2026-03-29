#pragma once
#include <algorithm>
#include "common.h"

template <u32 RBits, u32 GBits, u32 BBits>
class Rgb
{
public:
    u8 r, g, b;

    constexpr Rgb()
        : r(0), g(0), b(0) { }

    constexpr Rgb(u32 red, u32 green, u32 blue)
        : r(red), g(green), b(blue) { }

    constexpr Rgb(const Rgb<RBits, GBits, BBits>& rgb)
        : r(rgb.r), g(rgb.g), b(rgb.b) { }

    template <u32 RBits2, u32 GBits2, u32 BBits2>
    explicit constexpr Rgb(const Rgb<RBits2, GBits2, BBits2>& rgb)
    // {
    //     if (RBits2 > RBits)
    //         r = rgb.r >> (RBits2 - RBits);
    //     else if (RBits2 < RBits)
    //         r = rgb.r << (RBits - RBits2);
    //     else
    //         r = rgb.r;

    //     if (GBits2 > GBits)
    //         g = rgb.g >> (GBits2 - GBits);
    //     else if (GBits2 < GBits)
    //         g = rgb.g << (GBits - GBits2);
    //     else
    //         g = rgb.g;

    //     if (BBits2 > BBits)
    //         b = rgb.b >> (BBits2 - BBits);
    //     else if (BBits2 < BBits)
    //         b = rgb.b << (BBits - BBits2);
    //     else
    //         b = rgb.b;
    // }
        : r((rgb.r * ((1 << RBits) - 1) + ((1 << RBits2) >> 1)) / ((1 << RBits2) - 1))
        , g((rgb.g * ((1 << GBits) - 1) + ((1 << GBits2) >> 1)) / ((1 << GBits2) - 1))
        , b((rgb.b * ((1 << BBits) - 1) + ((1 << BBits2) >> 1)) / ((1 << BBits2) - 1)) { }

    /// @brief Returns this color with its components clamped to their allowed range. 
    /// @return The clamped color.
    constexpr Rgb Clamped() const
    {
        return Rgb(
            std::clamp<u32>(r, 0, (1 << RBits) - 1),
            std::clamp<u32>(g, 0, (1 << GBits) - 1),
            std::clamp<u32>(b, 0, (1 << BBits) - 1));
    }

    /// @brief Adds the \p other color and clamps the resulting rgb values to their allowed range.
    /// @param other The color to add.
    /// @return The result of adding the colors and clamping the result.
    constexpr Rgb operator+(const Rgb& other) const
    {
        return Rgb(
            std::clamp<u32>(r + other.r, 0, (1 << RBits) - 1),
            std::clamp<u32>(g + other.g, 0, (1 << GBits) - 1),
            std::clamp<u32>(b + other.b, 0, (1 << BBits) - 1));
    }

    /// @brief Adds the \p other color and without clamping the resulting rgb values.
    ///        Note that this can cause an overflow.
    /// @param other The color to add.
    /// @return The result of adding the colors.
    constexpr Rgb AddUnclamped(const Rgb& other) const
    {
        return Rgb(r + other.r, g + other.g, b + other.b);
    }
};

using Rgb8 = Rgb<8, 8, 8>;
