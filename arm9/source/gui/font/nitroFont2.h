#pragma once

#define NFT2_SIGNATURE      0x3254464E

struct nft2_glyph_t
{
    u32 dataOffset : 24;
    u32 glyphWidth : 8;
    s8 spacingLeft;
    s8 spacingRight;
    u8 glyphHeight;
    s8 spacingTop;
};

struct nft2_char_map_entry_t
{
    u16 count;
    u16 startChar;
    u16 glyphs[1];
};

struct nft2_header_t
{
    u32 signature;
    const nft2_glyph_t* glyphInfoPtr;
    const nft2_char_map_entry_t* charMapPtr;
    const u8* glyphDataPtr;
    u8 ascend;
    u8 descend;
    u16 glyphCount;
};

struct nft2_string_render_params_t
{
    int x;
    int y;
    u32 width;
    u32 height;
    u32 textWidth;
    // u32 textHeight;
    bool a5i3;
    bool onlyRenderWholeGlyphs = true;
};

/// @brief Prepares the ntf2 data of the given \p font for runtime use.
///        Call this method once after loading a font file.
/// @param font The font to prepare.
/// @return True if preparing was successful, or false otherwise.
bool nft2_unpack(nft2_header_t* font);

/// @brief Finds the glyph index in the given \p font that corresponds to the given \p character.
/// @param font The font the find the glyph index in.
/// @param character The character to find the glyph index for.
/// @return The glyph index if found, or 0 otherwise.
int nft2_findGlyphIdxForCharacter(const nft2_header_t* font, u16 character);

/// @brief Renders the given \p string with the given \p font to the \p dst buffer
///        with the given \p stride and \p renderParams.
/// @param font The font to use.
/// @param string The string to render.
/// @param dst The destination buffer.
/// @param stride The stride of the destination buffer.
/// @param renderParams The render params.
void nft2_renderString(const nft2_header_t* font, const char16_t* string, u8* dst,
    u32 stride, nft2_string_render_params_t* renderParams);
    
/// @brief Measures the given \p string with the given \p font. Returns the \p width and \p height.
/// @param font The font to use.
/// @param string The string to measure.
/// @param width The measured width.
/// @param height The measured height.
void nft2_measureString(const nft2_header_t* font, const char16_t* string, u32& width, u32& height);

/// @brief Renders the given \p string with the given \p font to the \p dst buffer
///        with the given \p stride and \p renderParams. If the string is too long to
///        fit the buffer, \p ellipsisString is rendered at 3/4rd of the string, cutting
///        it in two parts.
/// @param font The font to use.
/// @param string The string to render.
/// @param dst The destination buffer.
/// @param stride The stride of the destination buffer. 
/// @param renderParams The render params.
/// @param ellipsisString The ellipsis string.
void nft2_renderStringEllipsis(const nft2_header_t* font, const char16_t* string, u8* dst,
    u32 stride, nft2_string_render_params_t* renderParams, const char16_t* ellipsisString);