#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "left_icon.h"
#include "right_icon.h"
#include "SelectorFieldView.h"

#define VALUE_LABEL_OFFSET_X    60
#define LEFT_ARROW_OFFSET_X     40
#define RIGHT_ARROW_OFFSET_X    166
#define ARROW_OFFSET_Y          (-8)

SelectorFieldView::SelectorFieldView(const IFontRepository* fontRepository, const MaterialColorScheme* materialColorScheme)
    : _materialColorScheme(materialColorScheme)
    , _label(Label2DView::CreateShared(64, 16, 25, fontRepository->GetFont(FontType::Regular10)))
    , _valueLabel(Label2DView::CreateShared(120, 16, 20, fontRepository->GetFont(FontType::Regular10)))
{
    _valueLabel->SetHorizontalAlignment(Alignment::Center);
    _leftArrow = CreateArrow([] (IconButtonView* sender, void* arg)
    {
        reinterpret_cast<SelectorFieldView*>(arg)->Select(-1);
    });
    _rightArrow = CreateArrow([] (IconButtonView* sender, void* arg)
    {
        reinterpret_cast<SelectorFieldView*>(arg)->Select(1);
    });

    AddChildTail(_label.GetPointer());
    AddChildTail(_valueLabel.GetPointer());
    AddChildTail(_leftArrow.GetPointer());
    AddChildTail(_rightArrow.GetPointer());
}

SharedPtr<IconButton2DView> SelectorFieldView::CreateArrow(IconButtonView::button_action_t action)
{
    auto arrow = IconButton2DView::CreateShared(
        IconButtonView::Type::Standard,
        IconButtonView::State::ToggleUnselected,
        md::sys::color::surfaceContainerLow,
        _materialColorScheme);
    arrow->SetAction(action, this);
    return arrow;
}

SelectorFieldView::VramToken SelectorFieldView::UploadGraphics(IVramManager& vramManager)
{
    u32 leftOffset = vramManager.Alloc(left_iconTilesLen);
    dma_ntrCopy32(3, left_iconTiles, vramManager.GetVramAddress(leftOffset), left_iconTilesLen);
    u32 rightOffset = vramManager.Alloc(right_iconTilesLen);
    dma_ntrCopy32(3, right_iconTiles, vramManager.GetVramAddress(rightOffset), right_iconTilesLen);
    return VramToken(leftOffset, rightOffset);
}

void SelectorFieldView::SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken, const VramToken& vramToken)
{
    _leftArrow->SetGraphics(iconButtonVramToken);
    _rightArrow->SetGraphics(iconButtonVramToken);
    _leftArrow->SetIconVramOffset(vramToken.GetLeftArrowVramOffset());
    _rightArrow->SetIconVramOffset(vramToken.GetRightArrowVramOffset());
}

void SelectorFieldView::Select(int direction)
{
    if (_selectAction)
        _selectAction(direction, _selectActionArg);
}

void SelectorFieldView::Update()
{
    _label->SetPosition(_position.x, _position.y);
    _valueLabel->SetPosition(_position.x + VALUE_LABEL_OFFSET_X, _position.y);
    _leftArrow->SetPosition(_position.x + LEFT_ARROW_OFFSET_X, _position.y + ARROW_OFFSET_Y);
    _rightArrow->SetPosition(_position.x + RIGHT_ARROW_OFFSET_X, _position.y + ARROW_OFFSET_Y);

    if (!_valueLabel->IsFocused())
    {
        _leftArrow->SetState(IconButtonView::State::ToggleUnselected);
        _rightArrow->SetState(IconButtonView::State::ToggleUnselected);
    }

    ViewContainer::Update();
}

void SelectorFieldView::Draw(GraphicsContext& graphicsContext)
{
    const auto& backgroundColor = _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow);
    _label->SetBackgroundColor(backgroundColor);
    _label->SetForegroundColor(_materialColorScheme->onSurfaceVariant);

    bool focused = _valueLabel->IsFocused();
    _valueLabel->SetBackgroundColor(focused
        ? _materialColorScheme->GetColor(md::sys::color::secondaryContainer)
        : backgroundColor);
    _valueLabel->SetForegroundColor(focused
        ? _materialColorScheme->GetColor(md::sys::color::primary)
        : _materialColorScheme->onSurfaceVariant);

    ViewContainer::Draw(graphicsContext);
}

bool SelectorFieldView::HandleInput(const InputProvider& inputProvider, FocusManager& focusManager)
{
    // Highlight the arrow while its direction is held
    _leftArrow->SetState(inputProvider.Current(InputKey::DpadLeft)
        ? IconButtonView::State::ToggleSelected : IconButtonView::State::ToggleUnselected);
    _rightArrow->SetState(inputProvider.Current(InputKey::DpadRight)
        ? IconButtonView::State::ToggleSelected : IconButtonView::State::ToggleUnselected);

    return View::HandleInput(inputProvider, focusManager);
}

void SelectorFieldView::HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager)
{
    ViewContainer::HandlePenUp(lastTouchPoint, focusManager);

    // Tapping an arrow focuses it; keep the focus on the value instead
    if (_leftArrow->IsFocused() || _rightArrow->IsFocused())
        focusManager.Focus(_valueLabel);
}

SharedPtr<View> SelectorFieldView::MoveFocus(const SharedPtr<View>& currentFocus,
    FocusMoveDirection direction, View* source)
{
    if (currentFocus.GetPointer() == _valueLabel.GetPointer())
    {
        if (direction == FocusMoveDirection::Left)
        {
            Select(-1);
            return _valueLabel;
        }
        else if (direction == FocusMoveDirection::Right)
        {
            Select(1);
            return _valueLabel;
        }
    }
    return View::MoveFocus(currentFocus, direction, source);
}
