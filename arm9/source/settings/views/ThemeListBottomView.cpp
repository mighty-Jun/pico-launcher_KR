#include "common.h"
#include "gui/input/InputProvider.h"
#include "ThemeListBottomView.h"

ThemeListBottomView::ThemeListBottomView(SharedPtr<ThemeListViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
    const IRomBrowserViewFactory* romBrowserViewFactory, const IThemeFileIconFactory* themeFileIconFactory,
    VBlankTextureLoader* vblankTextureLoader)
    : _appBarView(SettingsAppBarView::CreateShared(viewModel, romBrowserViewFactory))
    , _recyclerView(RecyclerView::CreateShared(42, 0, 256 - 42, 192, RecyclerView::Mode::VerticalList))
    , _viewModel(std::move(viewModel))
{
    AddChildTail(_appBarView.GetPointer());
    AddChildTail(_recyclerView.GetPointer());
    _recyclerView->SetPadding(0, 3);
    _recyclerView->SetItemSpacing(0, 3);
    _themeAdapter = SharedPtr<ThemeAdapter>::MakeShared(
        _viewModel->GetSettingsController(), romBrowserViewFactory, themeFileIconFactory, vblankTextureLoader);
}

void ThemeListBottomView::InitVram(const VramContext& vramContext)
{
    _appBarView->InitVram(vramContext);
    _themeAdapter->InitVram(vramContext); // first initialize the shared vram for the items
    _recyclerView->SetAdapter(_themeAdapter, 0); // set the adapter of the recycler
    _recyclerView->InitVram(vramContext); // init the vram for the recycler and its items
}

void ThemeListBottomView::Update()
{
    ViewContainer::Update();
    _viewModel->SetSelectedItem(_recyclerView->GetSelectedItem());
}

SharedPtr<View> ThemeListBottomView::MoveFocus(const SharedPtr<View>& currentFocus, FocusMoveDirection direction, View* source)
{
    if (!currentFocus)
    {
        return nullptr;
    }
    if (source == _appBarView.GetPointer())
    {
        if (direction == FocusMoveDirection::Right)
        {
            return _recyclerView->MoveFocus(currentFocus, direction, this);
        }
        return nullptr;
    }
    else if (source == _recyclerView.GetPointer())
    {
        if (direction == FocusMoveDirection::Left)
        {
            return _appBarView->MoveFocus(currentFocus, direction, this);
        }
        return nullptr;
    }
    return nullptr;
}

bool ThemeListBottomView::HandleInput(const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->NavigateUp();
        return true;
    }
    return View::HandleInput(inputProvider, focusManager);
}
