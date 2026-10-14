#pragma once
#include "LabelView.h"
#include <libtwl/rtos/rtosMutex.h>
#include "gui/VBlankTextureLoader.h"

class alignas(32) Label3DView : public LabelView
{
    SHARED_ONLY(Label3DView)

public:
    ~Label3DView() override;

    void InitVram(const VramContext& vramContext) override;
    void Draw(GraphicsContext& graphicsContext) override;

private:
    Label3DView(u32 width, u32 height, u32 maxStringLength, const nft2_header_t* font,
        VBlankTextureLoader* vblankTextureLoader);

    void UpdateTileBuffer() override;

    u32 _texVramOffset = 0;
    VBlankTextureLoader* _vblankTextureLoader;
    VBlankTextureLoadRequest _textureLoadRequest;

    // UpdateTileBuffer runs on task threads (text set while binding list items) and on the
    // main thread (ellipsis/marquee changes); see UpdateTileBuffer.
    rtos_mutex_t _updateMutex;
};
