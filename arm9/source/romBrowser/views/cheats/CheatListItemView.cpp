#include "common.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "gui/palette/GradientPalette.h"
#include "gui/GraphicsContext.h"
#include "gui/OamBuilder.h"
#include "CheatListItemView.h"

#define ICON_X             4
#define ICON_Y             4

#define NAME_LABEL_X       24
#define NAME_LABEL_Y       5

CheatListItemView::CheatListItemView(const VramOffsets& vramOffsets,
    const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
    : _nameLabel(196, 16, 256, fontRepository->GetFont(FontType::Regular10))
    , _vramOffsets(vramOffsets)
    , _materialColorScheme(materialColorScheme)
{
    _nameLabel.SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    AddChildTail(&_nameLabel);
}

void CheatListItemView::Update()
{
    _nameLabel.SetPosition(_position.x + NAME_LABEL_X, _position.y + NAME_LABEL_Y);
    if (_cheatEntry != nullptr && !_cheatEntry->IsCheatCategory())
    {
        _iconVramOffset = _cheatEntry->GetIsCheatActive()
            ? _vramOffsets.checkboxCheckedIconVramOffset
            : _vramOffsets.checkboxUncheckedIconVramOffset;
    }
    if (IsFocused())
    {
        _nameLabel.SetEllipsisStyle(LabelView::EllipsisStyle::Marquee);
    }
    else
    {
        _nameLabel.SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    }
    ViewContainer::Update();
}

void CheatListItemView::Draw(GraphicsContext& graphicsContext)
{
    if (!graphicsContext.IsVisible(GetBounds()))
    {
        return;
    }

    auto backColor = _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow);
    if (IsFocused())
    {
        auto selectorFullColor = _materialColorScheme->GetColor(md::sys::color::onSurface);
        backColor = RgbMixer::Lerp(backColor, selectorFullColor, 10, 100);
    }

    _nameLabel.SetBackgroundColor(backColor);
    _nameLabel.SetForegroundColor(_materialColorScheme->onSurface);

    if (IsFocused())
    {
        u32 selectorPaletteRow = graphicsContext.GetPaletteManager().AllocRow(
            GradientPalette(backColor, backColor),
            _position.y, _position.y + 24);
        auto selectorOam = graphicsContext.GetOamManager().AllocOams(4);
        OamBuilder::OamWithSize<64, 32>(_position.x, _position.y, _vramOffsets.cheatSelectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[0]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 64, _position.y, _vramOffsets.cheatSelectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[1]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 2 * 64, _position.y, _vramOffsets.cheatSelectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[2]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 2 * 64 + 32, _position.y, _vramOffsets.cheatSelectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[3]);
    }

    ViewContainer::Draw(graphicsContext);

    if (graphicsContext.IsVisible(Rectangle(_position.x + ICON_X, _position.y + ICON_Y, 16, 16)))
    {
        u32 iconPaletteRow = graphicsContext.GetPaletteManager().AllocRow(
            GradientPalette(backColor, _materialColorScheme->primary),
            _position.y + ICON_Y, _position.y + ICON_Y + 16);
        auto iconOam = graphicsContext.GetOamManager().AllocOams(1);
        OamBuilder::OamWithSize<16, 16>(_position.x + ICON_X, _position.y + ICON_Y, _iconVramOffset >> 7)
            .WithPalette16(iconPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(iconOam[0]);
    }
}
