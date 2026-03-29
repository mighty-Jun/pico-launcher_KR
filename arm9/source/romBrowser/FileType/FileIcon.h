#pragma once
#include "core/math/Point.h"
#include "core/math/Rgb.h"

class PaletteManager;
class GraphicsContext;

#define FILE_ICON_VRAM_SIZE     1024

/// @brief Abstract base class representing a file icon.
class FileIcon
{
public:
    virtual ~FileIcon() = 0;

    /// @brief Sets the OBJ vram address and offset to use.
    /// @param objVramAddress The OBJ vram address to use.
    /// @param objVramOffset The OBJ vram offset to use.
    virtual void SetVramAddress(vu16* objVramAddress, u32 objVramOffset)
    {
        _vramAddress = objVramAddress;
        _vramOffset = objVramOffset;
    }

    /// @brief Uploads the graphics of this icon to the vram address specified by SetVramAddress.
    virtual void UploadGraphics() = 0;

    /// @brief Updates this icon.
    virtual void Update() { }

    /// @brief Draws this file icon.
    /// @param graphicsContext The graphics context to use.
    /// @param backgroundColor The color on which the icon is drawn.
    virtual void Draw(GraphicsContext& graphicsContext, const Rgb<8, 8, 8>& backgroundColor) = 0;

    /// @brief Sets the icon animation frame.
    /// @param frame The animation frame.
    void SetAnimFrame(u32 frame)
    {
        _frame = frame;
    }

    /// @brief Sets the position where this icon will be drawn.
    /// @param position The position where this icon will be drawn.
    void SetPosition(const Point& position) { _position = position; }

    /// @brief Sets the position where this icon will be drawn.
    /// @param x The X position where this icon will be drawn.
    /// @param y The Y position where this icon will be drawn.
    void SetPosition(int x, int y)
    {
        _position.x = x;
        _position.y = y;
    }

protected:
    vu16* _vramAddress = nullptr;
    u32 _vramOffset = 0;
    u32 _frame = 0;
    Point _position;
};

inline FileIcon::~FileIcon() { }
