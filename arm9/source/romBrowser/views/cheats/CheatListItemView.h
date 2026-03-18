#pragma once
#include "gui/views/ViewContainer.h"
#include "gui/views/Label2DView.h"
#include "cheats/CheatEntry.h"

class MaterialColorScheme;
class IFontRepository;

/// @brief List item view for the cheats panel, representing a single cheat or cheat category.
class CheatListItemView : public ViewContainer
{
public:
    struct VramOffsets
    {
        u32 folderIconVramOffset = 0;
        u32 checkboxUncheckedIconVramOffset = 0;
        u32 checkboxCheckedIconVramOffset = 0;
        u32 cheatSelectorVramOffset = 0;
    };

    CheatListItemView(const VramOffsets& vramOffsets, const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);

    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position.x, _position.y, 224, 24);
    }

    void SetName(const char* name)
    {
        _nameLabel.SetText(name);
    }

    void SetEntry(const CheatEntry* cheatEntry)
    {
        _cheatEntry = cheatEntry;
        _nameLabel.SetText(cheatEntry->GetName());
        if (cheatEntry->IsCheatCategory())
        {
            _iconVramOffset = _vramOffsets.folderIconVramOffset;
        }
    }

private:
    Label2DView _nameLabel;
    VramOffsets _vramOffsets;
    const MaterialColorScheme* _materialColorScheme;
    u32 _iconVramOffset = 0;
    const CheatEntry* _cheatEntry = nullptr;
};
