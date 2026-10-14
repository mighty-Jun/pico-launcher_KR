#pragma once
class LanguagePack;

class JsonLanguagePackSerializer
{
public:
    bool Deserialize(LanguagePack* languagePack, const char* filePath) const;
};