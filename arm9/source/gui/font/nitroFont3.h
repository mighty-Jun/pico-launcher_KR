#pragma once

// NitroFont3 포맷 고유 시그니처 (리틀 엔디안 기준 'NFT3')
#define NFT3_SIGNATURE      0x3354464E

/// @brief 개별 글자의 가로폭 및 VRAM 비트맵 데이터 위치 정보
struct nft3_glyph_t
{
    u32 dataOffset : 24; // 픽셀 데이터 시작 오프셋 (최대 16MB)
    u32 glyphWidth : 8;  // 글리프 실제 이미지 너비
    s8 spacingLeft;      // 좌측 여백
    s8 spacingRight;     // 우측 여백
    u8 glyphHeight;      // 글리프 실제 이미지 높이
    s8 spacingTop;       // 상단 여백 (Baseline 기준 Y 오프셋)
};

/// @brief [NFT3 핵심] 유니코드와 글리프 인덱스의 1:1 매핑 데이터 구조체 (4바이트 고정 크기)
struct nft3_char_map_t
{
    u16 unicode;   // 유니코드 문자 값 (오름차순 정렬됨)
    u16 glyphIdx;  // 대응하는 nft3_glyph_t 인덱스 번호
};

/// @brief NFT3 바이너리 파일 구조와 매핑되는 메인 헤더
struct nft3_header_t
{
    u32 signature;                      // "NFT3" 포맷 서명 검증용
    const nft3_glyph_t* glyphInfoPtr;  // 글리프 가로폭/여백 정보 배열 포인터
    const nft3_char_map_t* charMapPtr; // [NFT3] 1:1 매핑 테이블 배열 포인터
    const u8* glyphDataPtr;            // 오리지널 픽셀 비트맵 데이터 포인터
    u8 ascend;                          // 폰트 Ascender 높이
    u8 descend;                         // 폰트 Descender 높이
    u16 glyphCount;                    // 전체 글리프 이미지 개수
    u32 mappedCharCount;               // [NFT3] 매핑 테이블에 등록된 총 글자 수 (이진 탐색 범위)
};

/// @brief 문자열 출력 제어용 파라미터 구조체
struct nft3_string_render_params_t
{
    int x;
    int y;
    u32 width;
    u32 height;
    u32 textWidth;
    bool a5i3;                          // true: A5I3 텍스트 포맷, false: Tiled 포맷
    bool onlyRenderWholeGlyphs = true;  // 화면 경계에 걸친 글자를 자를지 여부
};

/// @brief 파일로부터 로드된 NFT3 폰트 데이터의 상대 오프셋 주소를 포인터 주소로 언팩합니다.
/// @param font 준비할 NFT3 폰트 데이터 헤더 포인터
/// @return 언팩 성공 시 true, 서명이 올바르지 않으면 false
bool nft3_unpack(nft3_header_t* font);

/// @brief 폰트 내 매핑 테이블을 이진 탐색하여 유니코드에 대응하는 글리프 인덱스를 반환합니다.
/// @param font 탐색할 NFT3 폰트 데이터 포인터
/// @param character 탐색할 유니코드 문자 (16비트)
/// @return 찾아낸 글리프 인덱스 번호 (없을 경우 0 반환)
int nft3_findGlyphIdxForCharacter(const nft3_header_t* font, u16 character);

/// @brief 지정된 버퍼에 문자열을 렌더링합니다.
void nft3_renderString(const nft3_header_t* font, const char16_t* string, u8* dst,
    u32 stride, nft3_string_render_params_t* renderParams);
    
/// @brief 문자열의 전체 가로/세로 출력 크기를 측정합니다.
void nft3_measureString(const nft3_header_t* font, const char16_t* string, u32& width, u32& height);

/// @brief 화면 너비를 초과하는 문자열의 3/4 지점에 말줄임표(...)를 삽입하여 분할 렌더링합니다.
void nft3_renderStringEllipsis(const nft3_header_t* font, const char16_t* string, u8* dst,
    u32 stride, nft3_string_render_params_t* renderParams, const char16_t* ellipsisString);