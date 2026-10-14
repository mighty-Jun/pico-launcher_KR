#pragma once
#include "IGameSettingsService.h"
#include "JsonGameSettingsSerializer.h"

class JsonGameSettingsService : public IGameSettingsService
{
    JsonGameSettingsSerializer _serializer;
    GameSettings _gameSettings;
public:
    JsonGameSettingsService();

    GameSettings& GetGameSettings() override { return _gameSettings; }
    const GameSettings& GetGameSettings() const override { return _gameSettings; }

    void Save() const override;
};
