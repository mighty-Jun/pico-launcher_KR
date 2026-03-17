#pragma once
#include "gui/views/RecyclerAdapter.h"
#include "cheats/CheatEntry.h"
#include "CheatListItemView.h"

/// @brief Recycler adapter for cheats.
class CheatsAdapter : public RecyclerAdapter
{
public:
    CheatsAdapter(const CheatEntry* cheatCategory, const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository, const CheatListItemView::VramOffsets& vramOffsets)
        : _cheatCategory(cheatCategory), _materialColorScheme(materialColorScheme)
        , _fontRepository(fontRepository), _vramOffsets(vramOffsets) { }

    u32 GetItemCount() const override
    {
        u32 numberOfSubEntries = 0;
        _cheatCategory->GetSubEntries(numberOfSubEntries);
        return numberOfSubEntries;
    }

    void GetViewSize(int& width, int& height) const override
    {
        width = 224;
        height = 24;
    }

    View* CreateView() const override
    {
        return new CheatListItemView(_vramOffsets, _materialColorScheme, _fontRepository);
    }

    void DestroyView(View* view) const override
    {
        delete (CheatListItemView*)view;
    }

    void BindView(View* view, int index) const override
    {
        auto listItemView = static_cast<CheatListItemView*>(view);
        u32 numberOfSubEntries = 0;
        auto subEntries = _cheatCategory->GetSubEntries(numberOfSubEntries);
        listItemView->SetEntry(&subEntries[index]);
    }

    void ReleaseView(View* view, int index) const override
    {
        // Nothing to do
    }

private:
    const CheatEntry* _cheatCategory;
    const MaterialColorScheme* _materialColorScheme;
    const IFontRepository* _fontRepository;
    CheatListItemView::VramOffsets _vramOffsets;
};
