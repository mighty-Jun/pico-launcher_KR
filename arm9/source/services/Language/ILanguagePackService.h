#pragma once
#include "LanguagePack.h"

class ILanguagePackService
{
public:
    virtual ~ILanguagePackService() { }

    virtual LanguagePack& GetLanguagePack() = 0;
    virtual const LanguagePack& GetLanguagePack() const = 0;
    virtual void Save() const = 0;
};