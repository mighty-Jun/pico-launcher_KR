#pragma once
class LanguagePack;

class JsonLanguagePackSerializer
{
public:
    void Serialize(const LanguagePack* languagePack, const char* filePath) const;
    bool Deserialize(LanguagePack* languagePack, const char* filePath) const;
};