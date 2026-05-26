#include "common.h"
#include "NotoSansJP-Regular-10_nft3.h"
#include "NotoSansJP-Medium-7_5_nft3.h"
#include "NotoSansJP-Medium-10_nft3.h"
#include "NotoSansJP-Medium-11_nft3.h"
#include "DefaultFontRepository.h"

const nft3_header_t* DefaultFontRepository::GetFont(FontType fontType) const
{
    switch (fontType)
    {
        case FontType::Regular10:
        {
            return (const nft3_header_t*)NotoSansJP_Regular_10_nft3;
        }
        case FontType::Medium7_5:
        {
            return (const nft3_header_t*)NotoSansJP_Medium_7_5_nft3;
        }
        case FontType::Medium10:
        {
            return (const nft3_header_t*)NotoSansJP_Medium_10_nft3;
        }
        case FontType::Medium11:
        {
            return (const nft3_header_t*)NotoSansJP_Medium_11_nft3;
        }
        default:
        {
            return nullptr;
        }
    }
}
