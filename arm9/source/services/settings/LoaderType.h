#pragma once
#include <strings.h>

/// @brief The loader used to boot an nds rom.
enum class LoaderType
{
    PicoLoader,
    NdsBootstrap
};

static inline const char* serializeLoaderType(LoaderType loaderType)
{
    switch (loaderType)
    {
        case LoaderType::NdsBootstrap:
            return "NDS_Bootstrap";
        case LoaderType::PicoLoader:
        default:
            return "Pico_Loader";
    }
}

static inline bool tryParseLoaderType(const char* loaderTypeString, LoaderType& loaderType)
{
    if (!loaderTypeString)
        return false;

    if (!strcasecmp(loaderTypeString, "Pico_Loader"))
        loaderType = LoaderType::PicoLoader;
    else if (!strcasecmp(loaderTypeString, "NDS_Bootstrap"))
        loaderType = LoaderType::NdsBootstrap;
    else
        return false;

    return true;
}
