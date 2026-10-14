#pragma once
#include "ILanguagePackService.h"
#include "JsonLanguagePackSerializer.h"
#include "LanguagePack.h"

class JsonLanguagePackService : public ILanguagePackService
{
    JsonLanguagePackSerializer _serializer;
    LanguagePack _languagePack;
public:
    void Load(const char* language) override;

    const LanguagePack& GetLanguagePack() const override { return _languagePack; }
};
