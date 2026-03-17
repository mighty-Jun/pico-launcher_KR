#include "common.h"
#include "JsonLanguagePackService.h"
#include "core/mini-printf.h"

JsonLanguagePackService::JsonLanguagePackService(IAppSettingsService& appSettingsService)
{
    // AppSettings에서 설정된 언어(예: "korean")를 가져와 경로를 동적으로 조립합니다.
    const char* currentLanguage = appSettingsService.GetAppSettings().language.GetString();
    mini_snprintf(_filePath, sizeof(_filePath), "/_pico/lang/%s.json", currentLanguage);

    if (!_serializer.Deserialize(&_languagePack, _filePath))
        Save();
}