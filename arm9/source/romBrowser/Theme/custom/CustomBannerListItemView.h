#pragma once
#include "gui/views/LabelView.h"
#include "../../views/BannerListItemView.h"

class MaterialColorScheme;
class IFontRepository;
class CustomThemeInfo;

class CustomBannerListItemView : public BannerListItemView
{
public:
    CustomBannerListItemView(const CustomThemeInfo* customThemeInfo, const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository, u32 texVramOffset, u32 plttVramOffset,
        u32 selectedTexVramOffset, u32 selectedPlttVramOffset, VBlankTextureLoader* vblankTextureLoader);

    void Draw(GraphicsContext& graphicsContext) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position, 203, 44);
    }

private:
    const CustomThemeInfo* _customThemeInfo;
    const MaterialColorScheme* _materialColorScheme;
    u32 _texVramOffset = 0;
    u32 _plttVramOffset = 0;
    u32 _selectedTexVramOffset = 0;
    u32 _selectedPlttVramOffset = 0;
};
