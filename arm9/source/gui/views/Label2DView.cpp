#include "common.h"
#include "gui/OamManager.h"
#include "gui/OamBuilder.h"
#include "gui/IVramManager.h"
#include "gui/VramContext.h"
#include "gui/GraphicsContext.h"
#include "core/StringUtil.h"
#include "gui/palette/GradientPalette.h"
#include "Label2DView.h"

void Label2DView::InitVram(const VramContext& vramContext)
{
    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        _vramOffset = objVramManager->Alloc((_actualWidth * _actualHeight) >> 1);
        _vramAddress = objVramManager->GetVramAddress(_vramOffset);
    }
}

void Label2DView::UpdateTileBuffer()
{
    LabelView::UpdateTileBuffer();
    _tileBufferUpdated = true;
}

void Label2DView::Draw(GraphicsContext& graphicsContext)
{
    if (!graphicsContext.IsVisible(GetBounds()))
        return;

    u32 hCellCount = _actualWidth >> 5;
    u32 vCellCount = _actualHeight >> 4;
    u32 cellCount = hCellCount * vCellCount;
    auto oams = graphicsContext.GetOamManager().AllocOams(cellCount);
    int xOffset = _position.x;

    int currentWidth = _tileBufferUpdated ? _newStringWidth : _stringWidth;

    if (_hAlign == Alignment::Center)
        xOffset += ((int)_width - currentWidth) / 2;
    else if (_hAlign == Alignment::End)
        xOffset += (int)_width - currentWidth;
    u32 paletteRow = graphicsContext.GetPaletteManager().AllocRow(
        GradientPalette(_backgroundColor, _foregroundColor), _position.y, _position.y + _height);
    u32 i = 0;
    for (u32 y = 0; y < vCellCount; y++)
    {
        for (u32 x = 0; x < hCellCount; x++)
        {
            OamBuilder::OamWithSize<32, 16>(
                    xOffset + x * 32,
                    _position.y + y * 16,
                    (_vramOffset + i * 256) >> 7)
                .WithPalette16(paletteRow)
                .WithPriority(graphicsContext.GetPriority())
                .Build(oams[i]);
            i++;
        }
    }
}

void Label2DView::VBlank()
{
    if (_tileBufferUpdated)
    {
        memcpy((void*)_vramAddress, _tileBuffer.get(), _tileBufferSize);
        _tileBufferUpdated = false;
        _stringWidth = _newStringWidth;
    }
}