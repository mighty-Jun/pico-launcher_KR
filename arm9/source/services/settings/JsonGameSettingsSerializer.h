#pragma once
class GameSettings;

class JsonGameSettingsSerializer
{
public:
    void Serialize(const GameSettings* gameSettings, const char* filePath) const;
    bool Deserialize(GameSettings* gameSettings, const char* filePath) const;
};
