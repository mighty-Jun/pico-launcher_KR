#pragma once
#include <memory>
#include "core/String.h"

class LanguagePack
{
public:
    String<char, 32> displaySettings_title = "Display Settings";
    String<char, 32> displaySettings_layout = "Layout";
    String<char, 32> displaySettings_sorting = "Sorting";
    String<char, 32> displaySettings_filters = "Filters";
    String<char, 32> displaySettings_theme = "Theme";
    String<char, 32> displaySettings_langugage = "langugage";
    String<char, 32> cheatSettings_title = "Cheat";
    String<char, 64> cheatSettings_noCheatsMsg = "No cheats found.";
};