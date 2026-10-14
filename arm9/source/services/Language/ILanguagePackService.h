#pragma once
#include "LanguagePack.h"

class ILanguagePackService
{
public:
    virtual ~ILanguagePackService() { }

    virtual const LanguagePack& GetLanguagePack() const = 0;
};