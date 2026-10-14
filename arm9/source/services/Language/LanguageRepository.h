#pragma once
#include <array>
#include "core/String.h"

/// @brief Lists the language packs available in /_pico/lang.
class LanguageRepository
{
public:
    static constexpr u32 MAX_LANGUAGES = 16;

    /// @brief Scans /_pico/lang for language packs. Does file io, so call it from the io thread.
    void Initialize();

    u32 GetLanguageCount() const { return _languageCount; }

    /// @brief Returns the language name at \p index, which is the file name without .json.
    const char* GetLanguage(u32 index) const { return _languages[index].GetString(); }

    /// @brief Returns the index of \p language, or -1 when it is not available.
    int FindLanguage(const char* language) const;

private:
    std::array<String<char, 32>, MAX_LANGUAGES> _languages;
    vu32 _languageCount = 0;
};
