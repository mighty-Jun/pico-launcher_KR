#pragma once
#include "gui/views/ViewContainer.h"
#include "gui/views/Label2DView.h"
#include "IconButton2DView.h"

class IFontRepository;
class IVramManager;
class MaterialColorScheme;

/// @brief A settings row showing "label  < value >", where the value is changed
///        with the left and right arrows (dpad or touch).
class SelectorFieldView : public ViewContainer
{
    SHARED_ONLY(SelectorFieldView)

public:
    class VramToken
    {
        u32 _leftArrowVramOffset;
        u32 _rightArrowVramOffset;
    public:
        VramToken()
            : _leftArrowVramOffset(0), _rightArrowVramOffset(0) { }

        VramToken(u32 leftArrowVramOffset, u32 rightArrowVramOffset)
            : _leftArrowVramOffset(leftArrowVramOffset), _rightArrowVramOffset(rightArrowVramOffset) { }

        constexpr u32 GetLeftArrowVramOffset() const { return _leftArrowVramOffset; }
        constexpr u32 GetRightArrowVramOffset() const { return _rightArrowVramOffset; }
    };

    /// @brief Called with -1 or 1 when the previous or next value should be selected.
    typedef void (*select_action_t)(int direction, void* arg);

    static VramToken UploadGraphics(IVramManager& vramManager);

    void SetGraphics(const IconButton2DView::VramToken& iconButtonVramToken, const VramToken& vramToken);
    void SetLabel(const char* text) { _label->SetText(text); }
    void SetValue(const char* text) { _valueLabel->SetText(text); }
    void SetValue(const char16_t* text) { _valueLabel->SetText(text); }

    void SetSelectAction(select_action_t action, void* arg)
    {
        _selectAction = action;
        _selectActionArg = arg;
    }

    /// @brief Returns the view that receives the focus for this field.
    const SharedPtr<Label2DView>& GetFocusView() const { return _valueLabel; }

    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position.x, _position.y - 8, 198, 32);
    }

private:
    const MaterialColorScheme* _materialColorScheme;
    SharedPtr<Label2DView> _label;
    SharedPtr<Label2DView> _valueLabel;
    SharedPtr<IconButton2DView> _leftArrow;
    SharedPtr<IconButton2DView> _rightArrow;
    select_action_t _selectAction = nullptr;
    void* _selectActionArg = nullptr;

    SelectorFieldView(const IFontRepository* fontRepository, const MaterialColorScheme* materialColorScheme);

    SharedPtr<IconButton2DView> CreateArrow(IconButtonView::button_action_t action);
    void Select(int direction);
};
