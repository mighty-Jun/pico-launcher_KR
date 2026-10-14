#pragma once
#include "LanguagePack.h"

class ILanguagePackService
{
public:
    virtual ~ILanguagePackService() { }

    /// @brief Loads the language pack for \p language (the file name in /_pico/lang without .json).
    ///        Strings missing from the file keep their English defaults.
    virtual void Load(const char* language) = 0;

    virtual const LanguagePack& GetLanguagePack() const = 0;
};
