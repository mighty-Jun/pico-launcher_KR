#pragma once
#include "GameSettings.h"

class IGameSettingsService
{
public:
    virtual ~IGameSettingsService() { }

    virtual GameSettings& GetGameSettings() = 0;
    virtual const GameSettings& GetGameSettings() const = 0;
    virtual void Save() const = 0;
};
