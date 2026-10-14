#include "common.h"
#include <memory>
#include "json/ArduinoJson.h"
#include "GameSettings.h"
#include "fat/File.h"
#include "JsonGameSettingsSerializer.h"

#pragma GCC optimize("Os")

#define JSON_RESERVED_SIZE  1024

#define KEY_LOADER          "loader"

static std::unique_ptr<u8[]> writeJson(const GameSettings* gameSettings, u32& length)
{
    const auto entries = gameSettings->GetEntries();
    const u32 entryCount = gameSettings->GetEntryCount();
    DynamicJsonDocument json(JSON_RESERVED_SIZE + entryCount * (2 * JSON_OBJECT_SIZE(1)));
    for (u32 i = 0; i < entryCount; i++)
    {
        json[entries[i].romPath.GetString()][KEY_LOADER] = serializeLoaderType(entries[i].loader);
    }

    u32 outputSize = measureJsonPretty(json);
    std::unique_ptr<u8[]> fileData(new(cache_align) u8[outputSize]);

    serializeJsonPretty(json, fileData.get(), outputSize);

    length = outputSize;
    return fileData;
}

void JsonGameSettingsSerializer::Serialize(const GameSettings* gameSettings, const char* filePath) const
{
    u32 length = 0;
    std::unique_ptr<u8[]> fileData = writeJson(gameSettings, length);

    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        LOG_ERROR("Couldn't open game settings file for writing\n");
        return;
    }

    u32 bytesWritten;
    if (file->Write(fileData.get(), length, bytesWritten) != FR_OK || bytesWritten != length)
    {
        LOG_ERROR("Error while writing game settings file\n");
        return;
    }

    LOG_DEBUG("Game settings file written\n");
}

static void readJson(GameSettings* gameSettings, const JsonDocument& json)
{
    for (auto item : json.as<JsonObjectConst>())
    {
        LoaderType loader;
        if (tryParseLoaderType(item.value()[KEY_LOADER].as<const char*>(), loader))
        {
            gameSettings->SetLoader(item.key().c_str(), loader);
        }
    }
}

bool JsonGameSettingsSerializer::Deserialize(GameSettings* gameSettings, const char* filePath) const
{
    const auto file = std::make_unique<File>();
    if (file->Open(filePath, FA_READ | FA_OPEN_EXISTING) != FR_OK)
        return false;

    u32 fileSize = file->GetSize();
    if (fileSize == 0)
        return false;

    std::unique_ptr<u8[]> fileData(new(cache_align) u8[fileSize]);
    u8* fileDataPtr = fileData.get();

    u32 bytesRead = 0;
    if (file->Read(fileDataPtr, fileSize, bytesRead) != FR_OK)
        return false;

    DynamicJsonDocument json(std::max<u32>(JSON_RESERVED_SIZE, fileSize * 2));
    if (deserializeJson(json, fileDataPtr, fileSize) != DeserializationError::Ok)
        return false;

    readJson(gameSettings, json);

    return true;
}
