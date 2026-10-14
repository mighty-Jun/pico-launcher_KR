#pragma once
#include "ILanguagePackService.h"
#include "JsonLanguagePackSerializer.h"
#include "LanguagePack.h"
#include "services/settings/IAppSettingsService.h"

class JsonLanguagePackService : public ILanguagePackService
{
    JsonLanguagePackSerializer _serializer;
    LanguagePack _languagePack;
public:
    explicit JsonLanguagePackService(IAppSettingsService& appSettingsService);

    const LanguagePack& GetLanguagePack() const override { return _languagePack; }
};