#pragma once
#include "gui/views/View.h"
#include "gui/views/Label2DView.h"

class IFontRepository;
class LanguagePack;
class MaterialColorScheme;

/// @brief Shown on the bottom screen while a rom is being prepared for nds-bootstrap.
class LoadingView : public View
{
public:
    LoadingView(const IFontRepository* fontRepository, const MaterialColorScheme* colorScheme,
        const LanguagePack& languagePack);

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;

    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

private:
    SharedPtr<Label2DView> _loadingLabel;

    u32 _bg1VramOffset = 0;
    u32 _bg2VramOffset = 0;
};
