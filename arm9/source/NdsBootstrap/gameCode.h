#pragma once

static inline consteval u32 GAMECODE(const char code[4])
{
    return code[0] | (code[1] << 8) | (code[2] << 16) | (code[3] << 24);
}

static inline consteval u32 GAMECODE_NO_REGION(const char code[3])
{
    return code[0] | (code[1] << 8) | (code[2] << 16);
}
