#include "common.h"
#include <bit>
#include "gui/IVramManager.h"
#include "gui/Gx.h"
#include "gui/VramContext.h"
#include "gui/GraphicsContext.h"
#include "core/StringUtil.h"
#include "Label3DView.h"

Label3DView::Label3DView(u32 width, u32 height, u32 maxStringLength, const nft2_header_t* font,
    VBlankTextureLoader* vblankTextureLoader)
    : LabelView(width, height, maxStringLength, font, true)
    , _vblankTextureLoader(vblankTextureLoader)
{
    rtos_createMutex(&_updateMutex);
}

Label3DView::~Label3DView()
{
    _vblankTextureLoader->CancelLoad(_textureLoadRequest);
}

void Label3DView::InitVram(const VramContext& vramContext)
{
    const auto texVramManager = vramContext.GetTexVramManager();
    if (texVramManager)
    {
        _texVramOffset = texVramManager->Alloc(_tileBufferSize);
    }
}

void Label3DView::UpdateTileBuffer()
{
    // The main thread has a higher priority than the task threads and can interrupt a task
    // thread halfway through this function. Without the lock the main thread could queue
    // _textureLoadRequest again, after which the task thread overwrites the request while it
    // is linked in the texture loader's list. That breaks the list and drops other labels'
    // pending uploads, leaving their text blank until they are re-rendered.
    rtos_lockMutex(&_updateMutex);
    {
        _vblankTextureLoader->CancelLoad(_textureLoadRequest);
        LabelView::UpdateTileBuffer();
        _textureLoadRequest = VBlankTextureLoadRequest(_tileBuffer.get(), _tileBufferSize,
            _texVramOffset, nullptr, 0, 0, nullptr, nullptr);
        _vblankTextureLoader->RequestLoad(_textureLoadRequest);
    }
    rtos_unlockMutex(&_updateMutex);
}

void Label3DView::Draw(GraphicsContext& graphicsContext)
{
    if (!graphicsContext.IsVisible(GetBounds()))
        return;

    Gx::MtxIdentity();
    Gx::PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGON_MODE_MODULATE, GX_DISPLAY_MODE_FRONT,
        false, false, false, GX_DEPTH_FUNC_LESS, false, 31, graphicsContext.GetPolygonId());
    Gx::TexImageParam(_texVramOffset >> 3, false, false, false, false, (GxTexSize)(std::bit_width(_actualWidth) - 4),
        GX_TEXSIZE_1024, GX_TEXFMT_A5I3, false, GX_TEXGEN_NONE);
    graphicsContext.GetRgb6Palette()->ApplyColor(Rgb<6, 6, 6>(_foregroundColor));
    Gx::Begin(GX_PRIMITIVE_QUAD);
    Gx::TexCoord(0, 0);
    REG_GX_VTX_16 = GX_VTX_PACK(_position.x << 6, _position.y << 3);
    REG_GX_VTX_16 = (200) << 6;
    Gx::TexCoord(0, (int)_height);
    REG_GX_VTX_16 = GX_VTX_PACK(_position.x << 6, (_position.y + _height) << 3);
    REG_GX_VTX_16 = (200) << 6;
    Gx::TexCoord((int)_width, (int)_height);
    REG_GX_VTX_16 = GX_VTX_PACK((_position.x + _width) << 6, (_position.y + _height) << 3);
    REG_GX_VTX_16 = (200) << 6;
    Gx::TexCoord((int)_width, 0);
    REG_GX_VTX_16 = GX_VTX_PACK((_position.x + _width) << 6, _position.y << 3);
    REG_GX_VTX_16 = (200) << 6;
    Gx::End();
}