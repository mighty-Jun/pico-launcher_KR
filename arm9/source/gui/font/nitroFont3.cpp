#include "common.h"
#include "nitroFont3.h"

bool nft3_unpack(nft3_header_t* font)
{
    if (font->signature != NFT3_SIGNATURE) {
        LOG_DEBUG("[ERROR] Signature mismatch!\n");
        return false;
    }

    font->glyphInfoPtr = (const nft3_glyph_t*)((u32)font + (u32)font->glyphInfoPtr);
    font->charMapPtr = (const nft3_char_map_t*)((u32)font + (u32)font->charMapPtr);
    font->glyphDataPtr = (const u8*)((u32)font + (u32)font->glyphDataPtr);

    return true;
}

int nft3_findGlyphIdxForCharacter(const nft3_header_t* font, u16 character)
{
    // 컨버터에 의해 파일 내부에 직렬화되어 구워진(Baked) 매핑 배열을 직접 가리킴
    const nft3_char_map_t* mapArray = font->charMapPtr;
    
    int left = 0;
    int right = (int)font->mappedCharCount - 1;

    // 완벽한 O(log N) 고정 크기 인덱스 기반 이진 탐색 수행
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        u16 midUnicode = mapArray[mid].unicode;

        if (midUnicode == character) {
            return mapArray[mid].glyphIdx; // 일치하는 글리프 인덱스 즉시 반환
        }
        else if (midUnicode < character) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    // 폰트 변환 시 누락되었거나 정의되지 않은 문자는 안전하게 0번(기본/공백) 인덱스로 처리
    return 0;
}

static inline void renderGlyph(const nft3_header_t* font, const nft3_glyph_t* glyph,
    int xPos, int yPos, const nft3_string_render_params_t* renderParams, u8* dst, u32 stride, bool a5i3)
{
    int yOffset = glyph->spacingTop;
    u32 xStart = xPos < 0 ? -xPos : 0;
    u32 yStart = yPos + yOffset < 0 ? -(yPos + yOffset) : 0;

    int xEnd = glyph->glyphWidth;
    if (xPos + xEnd > (int)renderParams->width)
    {
        if (renderParams->onlyRenderWholeGlyphs)
        {
            return;
        }
        xEnd = renderParams->width - xPos;
    }

    int yEnd = glyph->glyphHeight;
    if (yPos + yOffset + yEnd > (int)renderParams->height)
    {
        yEnd = renderParams->height - (yPos + yOffset); 
    }

    const u8* glyphData = &font->glyphDataPtr[glyph->dataOffset];
    glyphData += yStart * ((glyph->glyphWidth + 1) >> 1);
    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = xStart; x < xEnd; x++)
        {
            u32 data = glyphData[x >> 1];
            if ((x & 1) == 0)
                data &= 0xF;
            else
                data >>= 4;

            if (data == 0)
                continue;

            u32 finalX = x + xPos;
            u32 finalY = y + yPos + yOffset;

            if (a5i3)
            {
                dst[finalY * stride + finalX] = (data << 4) | (data ? (1 << 3) : 0);
            }
            else
            {
                u32 tileX = finalX >> 3;
                u32 tileY = finalY >> 3;

                u32 tileIdx = (tileY >> 1) * stride + (tileX >> 2) * 8 + (tileY & 1) * 4 + (tileX & 3);

                u32 offset = tileIdx * 64 + ((finalY & 7) << 3) + (finalX & 7);
                u32 value = dst[offset >> 1];
                if (offset & 1)
                {
                    u32 prevData = value >> 4;
                    if (prevData < data)
                        dst[offset >> 1] = (value & 0xF) | (data << 4);
                }
                else
                {
                    u32 prevData = value & 0xF;
                    if (prevData < data)
                        dst[offset >> 1] = (value & 0xF0) | data;
                }
            }
        }
        glyphData += (glyph->glyphWidth + 1) >> 1;
    }
}

static ITCM_CODE void renderGlyphTiled(const nft3_header_t* font, const nft3_glyph_t* glyph,
    int xPos, int yPos, const nft3_string_render_params_t* renderParams, u8* dst, u32 stride)
{
    renderGlyph(font, glyph, xPos, yPos, renderParams, dst, stride, false);
}

static ITCM_CODE void renderGlyphA5I3(const nft3_header_t* font, const nft3_glyph_t* glyph,
    int xPos, int yPos, const nft3_string_render_params_t* renderParams, u8* dst, u32 stride)
{
    renderGlyph(font, glyph, xPos, yPos, renderParams, dst, stride, true);
}

ITCM_CODE void nft3_renderString(const nft3_header_t* font, const char16_t* string, u8* dst, u32 stride,
    nft3_string_render_params_t* renderParams)
{
    int xPos = renderParams->x;
    int yPos = renderParams->y;
    bool a5i3 = renderParams->a5i3;
    u32 textWidth = 0;
    while (true)
    {
        u16 c = *string++;
        if (c == 0)
            break;
        if (c == '\n')
        {
            xPos = renderParams->x;
            yPos += font->ascend + font->descend + 1;
            if (yPos >= (int)renderParams->height)
                break;
            continue;
        }

        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft;
        if (a5i3)
        {
            renderGlyphA5I3(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        else
        {
            renderGlyphTiled(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        xPos += glyph->glyphWidth;
        if (xPos > (int)textWidth)
            textWidth = xPos;
        xPos += glyph->spacingRight;
    }
    renderParams->textWidth = textWidth - renderParams->x;
}

ITCM_CODE void nft3_measureString(const nft3_header_t* font, const char16_t* string, u32& width, u32& height)
{
    int xPos = 0;
    int yPos = 0;
    u32 textWidth = 0;
    while (true)
    {
        u16 c = *string++;
        if (c == 0)
            break;
        if (c == '\n')
        {
            xPos = 0;
            yPos += font->ascend + font->descend + 1;
            continue;
        }

        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft;
        xPos += glyph->glyphWidth;
        if (xPos > (int)textWidth)
            textWidth = xPos;
        xPos += glyph->spacingRight;
    }
    width = textWidth;
    height = yPos + font->ascend + font->descend;
}

static ITCM_CODE const char16_t* findFirstCharacterThatDoesNotFit(const nft3_header_t* font, const char16_t* string, u32 width)
{
    int xPos = 0;
    while (true)
    {
        u16 c = *string++;
        if (c == 0 || c == '\n')
            return string - 1;

        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft + glyph->glyphWidth + glyph->spacingRight;
        if (xPos > (int)width)
            return string - 1;
    }
}

static ITCM_CODE const char16_t* findLastCharacterThatFitsBackwards(const nft3_header_t* font, const char16_t* stringStart, const char16_t* stringEnd, u32 width)
{
    const char16_t* string = stringEnd;
    int xPos = (int)width;
    while (string >= stringStart)
    {
        u16 c = *--string;
        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        if (string != stringEnd - 1)
            xPos -= glyph->spacingRight;
        xPos -= glyph->glyphWidth;
        xPos -= glyph->spacingLeft;
        if (xPos < 0)
            return string + 1;
    }
    return stringStart;
}

static ITCM_CODE int measureEllipsisWidth(const nft3_header_t* font, const char16_t* ellipsisString)
{
    int ellipsisWidth = 0;
    const char16_t* stringPtr = ellipsisString;
    while (true)
    {
        u16 c = *stringPtr++;
        if (c == 0)
            break;

        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        ellipsisWidth += glyph->spacingLeft + glyph->glyphWidth + glyph->spacingRight;
    }
    return ellipsisWidth;
}

ITCM_CODE void nft3_renderStringEllipsis(const nft3_header_t* font, const char16_t* string, u8* dst,
    u32 stride, nft3_string_render_params_t* renderParams, const char16_t* ellipsisString)
{
    u32 stringWidth, stringHeight;
    nft3_measureString(font, string, stringWidth, stringHeight);
    if (stringWidth <= renderParams->width)
    {
        nft3_renderString(font, string, dst, stride, renderParams);
        return;
    }
    u32 splitPoint = (renderParams->width - renderParams->x) * 3 / 4;
    int ellipsisWidth = measureEllipsisWidth(font, ellipsisString);
    u32 splitLeftEnd = splitPoint - (ellipsisWidth >> 1);
    u32 splitRightStart = splitPoint + ((ellipsisWidth + 1) >> 1);
    const char16_t* endOfFirstPart = findFirstCharacterThatDoesNotFit(font, string, splitLeftEnd);
    u32 stringLength = 0;
    while (string[stringLength] != 0)
        stringLength++;
    const char16_t* startOfSecondPart = findLastCharacterThatFitsBackwards(
        font, endOfFirstPart, string + stringLength, renderParams->width - renderParams->x - splitRightStart);

    int xPos = renderParams->x;
    int yPos = renderParams->y;
    bool a5i3 = renderParams->a5i3;
    u32 textWidth = 0;
    const char16_t* stringPtr = string;
    while (stringPtr < endOfFirstPart)
    {
        u16 c = *stringPtr++;
        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft;
        if (a5i3)
        {
            renderGlyphA5I3(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        else
        {
            renderGlyphTiled(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        xPos += glyph->glyphWidth;
        if (xPos > (int)textWidth)
            textWidth = xPos;
        xPos += glyph->spacingRight;
    }
    stringPtr = ellipsisString;
    while (true)
    {
        u16 c = *stringPtr++;
        if (c == 0)
            break;
        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft;
        if (a5i3)
        {
            renderGlyphA5I3(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        else
        {
            renderGlyphTiled(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        xPos += glyph->glyphWidth;
        if (xPos > (int)textWidth)
            textWidth = xPos;
        xPos += glyph->spacingRight;
    }
    stringPtr = startOfSecondPart;
    while (true)
    {
        u16 c = *stringPtr++;
        if (c == 0)
            break;
        int glyphIdx = nft3_findGlyphIdxForCharacter(font, c);
        const nft3_glyph_t* glyph = &font->glyphInfoPtr[glyphIdx];
        xPos += glyph->spacingLeft;
        if (a5i3)
        {
            renderGlyphA5I3(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        else
        {
            renderGlyphTiled(font, glyph, xPos, yPos, renderParams, dst, stride);
        }
        xPos += glyph->glyphWidth;
        if (xPos > (int)textWidth)
            textWidth = xPos;
        xPos += glyph->spacingRight;
    }
    renderParams->textWidth = textWidth - renderParams->x;
}