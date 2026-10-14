#include "common.h"
#include "JsonLanguagePackService.h"
#include "core/mini-printf.h"

JsonLanguagePackService::JsonLanguagePackService(IAppSettingsService& appSettingsService)
{
    // AppSettings에서 설정된 언어(예: "korean")를 가져와 경로를 동적으로 조립합니다.
    const char* currentLanguage = appSettingsService.GetAppSettings().language.GetString();
    char filePath[64];
    mini_snprintf(filePath, sizeof(filePath), "/_pico/lang/%s.json", currentLanguage);

    // When the file is missing or invalid, the English defaults in LanguagePack are used.
    if (!_serializer.Deserialize(&_languagePack, filePath))
        LOG_WARNING("Failed to load language pack %s\n", filePath);
}