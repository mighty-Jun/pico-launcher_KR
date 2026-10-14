#include "common.h"
#include <libtwl/dma/dmaNitro.h>
#include "gui/PaletteManager.h"
#include "gui/OamManager.h"
#include "gui/OamBuilder.h"
#include "gui/GraphicsContext.h"
#include "gui/palette/DirectPalette.h"
#include "NdsFileIcon.h"

static uint16_t sCrc16Table[] =
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500,
    0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1,
    0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81,
    0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540,
    0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001,
    0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0,
    0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80,
    0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700,
    0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0,
    0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480,
    0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41,
    0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01,
    0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
    0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181,
    0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901,
    0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1,
    0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680,
    0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

static u16 getCrc16(const void* data, u32 length)
{
    u16 result = 0xFFFF;
    for (u32 i = 0; i < length; i++)
    {
        result = (result >> 8) ^ sCrc16Table[(result ^ ((u8*)data)[i]) & 0xFF];
    }

    return result;
}

NdsFileIcon::NdsFileIcon(const nds_banner_t* banner)
    : _banner(banner), _animTokenIdx(0), _lastAnimToken(0)
    , _animLength(0), _loop(false)
{
    _animated = _banner->header.version >= NDS_BANNER_VERSION_103
             && _banner->animation.animTokens[0].duration != 0
             && getCrc16(&_banner->animation, sizeof(_banner->animation)) == _banner->header.version103AnimCrc;
    if (_animated)
    {
        _lastAnimToken = NDS_BANNER_ANIM_TOKEN_COUNT - 1;
        int length = 0;
        _loop = true;
        for (int i = 0; i < NDS_BANNER_ANIM_TOKEN_COUNT; i++)
        {
            _tokenStartTimes[i] = length;
            const auto& token = _banner->animation.animTokens[i];
            if (token.duration == NDS_BANNER_ANIM_DURATION_CONTROL_FRAME)
            {
                _loop = token.control != NDS_BANNER_ANIM_CONTROL_STOP;
                _lastAnimToken = i - 1;
                break;
            }
            else
            {
                length += token.duration;
            }
        }
        _animLength = length;
        _tokenStartTimes[NDS_BANNER_ANIM_TOKEN_COUNT] = _animLength;
    }
}

void NdsFileIcon::SetVramAddress(vu16* objVramAddress, u32 objVramOffset)
{
    FileIcon::SetVramAddress(objVramAddress, objVramOffset);
    _currentVramSlot = 0;
    _currentGfxIdx = -1;
}

void NdsFileIcon::UploadGraphics()
{
    if (_vramAddress != nullptr && !_animated)
    {
        dma_ntrCopy32(3, _banner->iconGfx, _vramAddress, sizeof(_banner->iconGfx));
    }
}

void NdsFileIcon::Update()
{
    if (!_animated)
    {
        return;
    }

    _frame %= _animLength;

    if ((!_loop && _frame >= _animLength) || _lastAnimToken == 0)
    {
        _animTokenIdx = _lastAnimToken;
    }
    else
    {
        u32 start = 0;
        u32 end = _lastAnimToken;

        while (start <= end)
        {
            const u32 mid = (start + end) >> 1;
            u32 midTime = _tokenStartTimes[mid];
            if (midTime <= _frame && _frame < _tokenStartTimes[mid + 1])
            {
                start = mid;
                break;
            }
            else if (_frame < midTime)
            {
                end = mid - 1;
            }
            else
            {
                start = mid + 1;
            }
        }

        _animTokenIdx = start;
    }

    if (++_frame == _animLength)
    {
        _frame = 0;
    }
}

void NdsFileIcon::Draw(GraphicsContext& graphicsContext, const Rgb<8, 8, 8>& backgroundColor)
{
    if (!graphicsContext.IsVisible(Rectangle(_position, 32, 32)) ||
        _vramAddress == nullptr)
    {
        return;
    }

    const u16* palette = _animated
        ? _banner->animation.iconPltt[_banner->animation.animTokens[_animTokenIdx].plttIdx]
        : _banner->iconPltt;

    u32 paletteRowIdx = graphicsContext.GetPaletteManager().AllocRow(
        DirectPalette(palette), _position.y, _position.y + 32);

    u32 vramOffset = _vramOffset;
    if (_animated)
    {
        int gfxIdx = _banner->animation.animTokens[_animTokenIdx].gfxIdx;
        if (gfxIdx != _currentGfxIdx)
        {
            _currentVramSlot = 1 - _currentVramSlot;
            _currentGfxIdx = gfxIdx;
            dma_ntrCopy32(3, &_banner->animation.iconGfx[gfxIdx][0], (u8*)_vramAddress + (_currentVramSlot * NDS_BANNER_ICON_SIZE), NDS_BANNER_ICON_SIZE);
        }

        vramOffset += _currentVramSlot * NDS_BANNER_ICON_SIZE;
    }

    auto builder = OamBuilder::OamWithSize<32, 32>(
            _position, vramOffset >> 7)
        .WithPalette16(paletteRowIdx)
        .WithPriority(graphicsContext.GetPriority());

    if (_animated)
    {
        builder
            .WithHFlip(_banner->animation.animTokens[_animTokenIdx].hFlip)
            .WithVFlip(_banner->animation.animTokens[_animTokenIdx].vFlip);
    }

    gfx_oam_entry_t* oam = graphicsContext.GetOamManager().AllocOams(1);
    builder.Build(oam[0]);
}