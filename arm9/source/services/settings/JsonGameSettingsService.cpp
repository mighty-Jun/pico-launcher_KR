#include "common.h"
#include "JsonGameSettingsService.h"

#define GAME_SETTINGS_PATH  "/_pico/gameSettings.json"

JsonGameSettingsService::JsonGameSettingsService()
{
    // The file only exists once a game got its own settings, so a missing file is not an error.
    _serializer.Deserialize(&_gameSettings, GAME_SETTINGS_PATH);
}

void JsonGameSettingsService::Save() const
{
    _serializer.Serialize(&_gameSettings, GAME_SETTINGS_PATH);
}
