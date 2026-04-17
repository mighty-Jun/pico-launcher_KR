#pragma once
#include <memory>
#include "core/SharedPtr.h"
#include "romBrowser/views/BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "gui/views/RecyclerView.h"
#include "romBrowser/viewModels/CheatsViewModel.h"
#include "CheatsAdapter.h"
#include "CheatListItemView.h"

class MaterialColorScheme;
class IFontRepository;
class IVramManager;
class ILanguagePackService;

/// @brief Bottom sheet for browsing and enabling/disabling cheats.
class CheatsBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(CheatsBottomSheetView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenDown(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenMove(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;

    void Focus(FocusManager& focusManager) override
    {
        _cheatListRecycler->Focus(focusManager);
    }

private:
    SharedPtr<CheatsViewModel> _viewModel;
    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<Label2DView> _secondaryLabel;
    SharedPtr<Label2DView> _descriptionLabel;
    SharedPtr<RecyclerView> _cheatListRecycler;
    SharedPtr<CheatsAdapter> _cheatsAdapter;
    const MaterialColorScheme* _materialColorScheme;
    const IFontRepository* _fontRepository;
    IVramManager* _objVramManager = nullptr;
    FocusManager* _focusManager;
    CheatListItemView::VramOffsets _vramOffsets;
    u32 _savedVramState = 0;
    const CheatEntry* _currentCheatCategory = nullptr;
    bool _oobPenDown = false;

    CheatsBottomSheetView(SharedPtr<CheatsViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
        FocusManager* focusManager, ILanguagePackService* languagePackService);
    ILanguagePackService* _languagePackService;

    void UpdateCheatList();
    void UpdateDescriptionText();
};
