#pragma once
#include "ILanguagePackService.h"
#include "JsonLanguagePackSerializer.h"
#include "LanguagePack.h"
#include "services/settings/IAppSettingsService.h"

class JsonLanguagePackService : public ILanguagePackService
{
    JsonLanguagePackSerializer _serializer;
    LanguagePack _languagePack;
    char _filePath[64];
public:
    explicit JsonLanguagePackService(IAppSettingsService& appSettingsService);

    LanguagePack& GetLanguagePack() override { return _languagePack; }
    const LanguagePack& GetLanguagePack() const override { return _languagePack; }

    void Save() const override
    {
        _serializer.Serialize(&_languagePack, _filePath);
    }
};