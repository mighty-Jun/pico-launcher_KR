#include "common.h"
#include <string.h>
#include <strings.h>
#include "fat/Directory.h"
#include "LanguageRepository.h"

void LanguageRepository::Initialize()
{
    u32 count = 0;
    Directory directory;
    if (directory.Open("/_pico/lang") == FR_OK)
    {
        FILINFO fileInfo;
        while (count < MAX_LANGUAGES && directory.Read(&fileInfo) == FR_OK && fileInfo.fname[0] != 0)
        {
            if (fileInfo.fname[0] == '.' || (fileInfo.fattrib & AM_DIR))
                continue;

            char* dot = strrchr(fileInfo.fname, '.');
            if (!dot || strcasecmp(dot, ".json") != 0)
                continue;

            *dot = 0;
            _languages[count++] = (const char*)fileInfo.fname;
        }
    }

    if (count == 0)
    {
        _languages[0] = "english";
        count = 1;
    }

    // published last, the ui thread may already be reading the count
    _languageCount = count;
}

int LanguageRepository::FindLanguage(const char* language) const
{
    for (u32 i = 0; i < _languageCount; i++)
    {
        if (!strcasecmp(_languages[i].GetString(), language))
            return i;
    }
    return -1;
}
