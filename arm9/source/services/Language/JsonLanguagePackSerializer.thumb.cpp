#include "common.h"
#include <memory>
#include "json/ArduinoJson.h"
#include "LanguagePack.h"
#include "fat/File.h"
#include "JsonLanguagePackSerializer.h"
#include <nds.h>

#pragma GCC optimize("Os")

#define JSON_RESERVED_SIZE  1024 

// JSON 파일에서 사용할 키 이름 정의
#define KEY_DISPLAY_SETTINGS_TITLE   "displaySettings.title"
#define KEY_DISPLAY_SETTINGS_LAYOUT  "displaySettings.layout"
#define KEY_DISPLAY_SETTINGS_SORTING "displaySettings.sorting"
#define KEY_DISPLAY_SETTINGS_FILTERS "displaySettings.filters"
#define KEY_DISPLAY_SETTINGS_THEME "displaySettings.theme"
#define KEY_DISPLAY_SETTINGS_LANGUAGE "displaySettings.language"
#define KEY_CHEAT_SETTINGS_TITLE     "cheatSettings.title"
#define KEY_CHEAT_SETTINGS_NO_CHEATS "cheatSettings.noCheatsMsg"
#define KEY_LAUNCH_SETTINGS_TITLE "launchSettings.title"
#define KEY_LAUNCH_SETTINGS_LOADER "launchSettings.loader"

static std::unique_ptr<u8[]> writeJson(const LanguagePack* languagePack, u32& length)
{
    DynamicJsonDocument json(JSON_RESERVED_SIZE);

    // LanguagePack 구조체의 데이터를 JSON 객체에 매핑
    json[KEY_DISPLAY_SETTINGS_TITLE] = languagePack->displaySettings_title.GetString();
    json[KEY_DISPLAY_SETTINGS_LAYOUT] = languagePack->displaySettings_layout.GetString();
    json[KEY_DISPLAY_SETTINGS_SORTING] = languagePack->displaySettings_sorting.GetString();
    json[KEY_DISPLAY_SETTINGS_FILTERS] = languagePack->displaySettings_filters.GetString();
    json[KEY_DISPLAY_SETTINGS_THEME] = languagePack->displaySettings_theme.GetString();
    json[KEY_DISPLAY_SETTINGS_LANGUAGE] = languagePack->displaySettings_langugage.GetString();
    json[KEY_LAUNCH_SETTINGS_TITLE] = languagePack->launchSettings_title.GetString();
    json[KEY_LAUNCH_SETTINGS_LOADER] = languagePack->launchSettings_loader.GetString();

    u32 outputSize = measureJsonPretty(json);
    std::unique_ptr<u8[]> fileData(new(cache_align) u8[outputSize]);

    serializeJsonPretty(json, fileData.get(), outputSize);

    length = outputSize;
    return fileData;
}

void JsonLanguagePackSerializer::Serialize(const LanguagePack* languagePack, const char* filePath) const
{
    u32 length = 0;
    std::unique_ptr<u8[]> fileData = writeJson(languagePack, length);

    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        LOG_ERROR("Couldn't open language pack file for writing\n");
        return;
    }

    // SD 카드에 쓰기 전 메모리 상태 보장
    DC_FlushRange(fileData.get(), length);

    u32 bytesWritten;
    if (file->Write(fileData.get(), length, bytesWritten) != FR_OK || bytesWritten != length)
    {
        LOG_ERROR("Error while writing language pack file\n");
        return;
    }

    LOG_DEBUG("Language pack file written\n");
}

static void readJson(LanguagePack* languagePack, const JsonDocument& json)
{
    // JSON에 해당 키가 없으면 원래 LanguagePack.h에 있던 기본 영문값을 가져옴
    languagePack->displaySettings_title = json[KEY_DISPLAY_SETTINGS_TITLE] | languagePack->displaySettings_title.GetString();
    languagePack->displaySettings_layout = json[KEY_DISPLAY_SETTINGS_LAYOUT] | languagePack->displaySettings_layout.GetString();
    languagePack->displaySettings_sorting = json[KEY_DISPLAY_SETTINGS_SORTING] | languagePack->displaySettings_sorting.GetString();
    languagePack->displaySettings_filters = json[KEY_DISPLAY_SETTINGS_FILTERS] | languagePack->displaySettings_filters.GetString();
    languagePack->displaySettings_theme = json[KEY_DISPLAY_SETTINGS_THEME] | languagePack->displaySettings_theme.GetString();
    languagePack->displaySettings_langugage = json[KEY_DISPLAY_SETTINGS_LANGUAGE] | languagePack->displaySettings_theme.GetString();
    languagePack->cheatSettings_title = json[KEY_CHEAT_SETTINGS_TITLE] | languagePack->cheatSettings_title.GetString();
    languagePack->cheatSettings_noCheatsMsg = json[KEY_CHEAT_SETTINGS_NO_CHEATS] | languagePack->cheatSettings_noCheatsMsg.GetString();
    languagePack->launchSettings_title = json[KEY_LAUNCH_SETTINGS_TITLE] | languagePack->launchSettings_title.GetString();
    languagePack->launchSettings_loader = json[KEY_LAUNCH_SETTINGS_LOADER] | languagePack->launchSettings_loader.GetString();
}

bool JsonLanguagePackSerializer::Deserialize(LanguagePack* languagePack, const char* filePath) const
{
    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_READ | FA_OPEN_EXISTING) != FR_OK)
        return false;

    u32 fileSize = file->GetSize();
    if (fileSize == 0)
        return false;

    u32 alignedSize = (fileSize + 1 + 31) & ~31;
    std::unique_ptr<u8[]> fileData(new(cache_align) u8[alignedSize]);
    u8* fileDataPtr = fileData.get();

    DC_FlushRange(fileDataPtr, alignedSize);

    u32 bytesRead = 0;
    if (file->Read(fileDataPtr, fileSize, bytesRead) != FR_OK)
        return false;

    DC_InvalidateRange(fileDataPtr, alignedSize);

    fileDataPtr[fileSize] = '\0'; 

    DynamicJsonDocument json(std::max<u32>(JSON_RESERVED_SIZE, fileSize * 2));
    if (deserializeJson(json, fileDataPtr, fileSize) != DeserializationError::Ok)
        return false;

    readJson(languagePack, json);

    return true;
}