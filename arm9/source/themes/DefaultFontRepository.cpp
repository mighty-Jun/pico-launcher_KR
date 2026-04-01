#include "common.h"
#include "NotoSansJP-Regular-10_nft2.h"
#include "NotoSansJP-Medium-7_5_nft2.h"
#include "NotoSansJP-Medium-10_nft2.h"
#include "NotoSansJP-Medium-11_nft2.h"
#include "DefaultFontRepository.h"

static const nft2_header_t* s_fontRegular10 = nullptr;
static const nft2_header_t* s_fontMedium7_5 = nullptr;
static const nft2_header_t* s_fontMedium10 = nullptr;
static const nft2_header_t* s_fontMedium11 = nullptr;

void DefaultFontRepository::SetFont(FontType fontType, const nft2_header_t* fontPtr)
{
    switch (fontType)
    {
        case FontType::Regular10: s_fontRegular10 = fontPtr; break;
        case FontType::Medium7_5: s_fontMedium7_5 = fontPtr; break;
        case FontType::Medium10:  s_fontMedium10 = fontPtr; break;
        case FontType::Medium11:  s_fontMedium11 = fontPtr; break;
    }
}

const nft2_header_t* DefaultFontRepository::GetFont(FontType fontType) const
{
    switch (fontType)
    {
        case FontType::Regular10:
        {
            return s_fontRegular10;
        }
        case FontType::Medium7_5:
        {
            return s_fontMedium7_5;
        }
        case FontType::Medium10:
        {
            return s_fontMedium10;
        }
        case FontType::Medium11:
        {
            return s_fontMedium11;
        }
        default:
        {
            return nullptr;
        }
    }
}