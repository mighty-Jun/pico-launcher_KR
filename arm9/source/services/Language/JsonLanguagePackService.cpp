#include "common.h"
#include "JsonLanguagePackService.h"
#include "core/mini-printf.h"

void JsonLanguagePackService::Load(const char* language)
{
    _languagePack = LanguagePack();

    char filePath[64];
    mini_snprintf(filePath, sizeof(filePath), "/_pico/lang/%s.json", language);

    // When the file is missing or invalid, the English defaults in LanguagePack are used.
    if (!_serializer.Deserialize(&_languagePack, filePath))
        LOG_WARNING("Failed to load language pack %s\n", filePath);
}
