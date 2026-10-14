#include "common.h"
#include "core/Environment.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "gui/OamBuilder.h"
#include "gui/VramContext.h"
#include "gui/palette/DirectPalette.h"
#include "sharedMemory.h"
#include "battery.h"
#include "BatteryIndicatorView.h"

#define FRAME_SIZE              (32 * 16 / 2)
#define FRAME_DS_LOW            0
#define FRAME_DS_OK             1
#define FRAME_DSI_FIRST_LEVEL   2 // 2-5: one to four bars
#define FRAME_CHARGING          6
#define UPDATE_INTERVAL_FRAMES  60

BatteryIndicatorView::BatteryIndicatorView()
{
    UpdateFrame();
}

void BatteryIndicatorView::InitVram(const VramContext& vramContext)
{
    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        _vramOffset = objVramManager->Alloc(batteryTilesLen);
        dma_ntrCopy32(3, batteryTiles, objVramManager->GetVramAddress(_vramOffset), batteryTilesLen);
    }
}

void BatteryIndicatorView::Update()
{
    if (++_updateCounter >= UPDATE_INTERVAL_FRAMES)
    {
        _updateCounter = 0;
        UpdateFrame();
    }
}

void BatteryIndicatorView::UpdateFrame()
{
    u32 value = SHARED_BATTERY_LEVEL;
    u32 level = value & SHARED_BATTERY_LEVEL_MASK;

    if (value & SHARED_BATTERY_CHARGER_CONNECTED)
    {
        _frame = FRAME_CHARGING;
    }
    else if (Environment::IsDsiMode())
    {
        // DSi reports 0-15, shown as one to four bars
        int bars = level / 4;
        if (bars > 3)
            bars = 3;
        _frame = FRAME_DSI_FIRST_LEVEL + bars;
    }
    else
    {
        // DS only reports low (< 4) or okay
        _frame = level > 3 ? FRAME_DS_OK : FRAME_DS_LOW;
    }
}

void BatteryIndicatorView::Draw(GraphicsContext& graphicsContext)
{
    u32 paletteRow = graphicsContext.GetPaletteManager().AllocRow(
        DirectPalette((const u16*)batteryPal), _position.y, _position.y + 16);
    gfx_oam_entry_t* oam = graphicsContext.GetOamManager().AllocOams(1);
    OamBuilder::OamWithSize<32, 16>(_position.x, _position.y, (_vramOffset + _frame * FRAME_SIZE) >> 7)
        .WithPalette16(paletteRow)
        .WithPriority(graphicsContext.GetPriority())
        .Build(oam[0]);
}
