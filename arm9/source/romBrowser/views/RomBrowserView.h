#pragma once
#include "core/SharedPtr.h"
#include "../viewModels/RomBrowserViewModel.h"
#include "gui/views/RecyclerViewBase.h"
#include "../FileRecyclerAdapter.h"
#include "gui/views/View.h"
#include "../DisplayMode/RomBrowserDisplayMode.h"
#include "../Theme/IThemeFileIconFactory.h"

class IRomBrowserViewFactory;

class RomBrowserView : public View
{
public:
    RomBrowserView(
        SharedPtr<RomBrowserViewModel> viewModel,
        const RomBrowserDisplayMode& displayMode,
        const IThemeFileIconFactory* themeFileIconFactory,
        const IRomBrowserViewFactory* romBrowserViewFactory,
        VBlankTextureLoader* vblankTextureLoader);

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;

    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

    bool Focus(FocusManager& focusManager)
    {
        if (!_fileRecyclerAdapter || _fileRecyclerAdapter->GetItemCount() == 0)
            return false;

        _fileGridView->Focus(focusManager);
        return true;
    }

    SharedPtr<View> MoveFocus(
        const SharedPtr<View>& currentFocus, FocusMoveDirection direction, View* source) override;

    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;

private:
    SharedPtr<RomBrowserViewModel> _viewModel;
    SharedPtr<RecyclerViewBase> _fileGridView;
    SharedPtr<FileRecyclerAdapter> _fileRecyclerAdapter;
    bool _isVertical;
};
