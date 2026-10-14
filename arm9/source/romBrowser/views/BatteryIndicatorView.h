#pragma once
#include "gui/views/View.h"

/// @brief Battery icon in the top right corner of the top screen.
class BatteryIndicatorView : public View
{
public:
    BatteryIndicatorView();

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position.x, _position.y, 32, 16);
    }

private:
    u32 _vramOffset = 0;
    int _frame = 0;
    int _updateCounter = 0;

    void UpdateFrame();
};
