#include "common.h"
#include "SaveListFactory.h"

std::unique_ptr<SaveList> SaveListFactory::CreateFromFile(const TCHAR* path)
{
    FIL file;
    if (f_open(&file, path, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    {
        LOG_FATAL("Failed to open save list file\n");
        return nullptr;
    }
    const u32 entryCount = f_size(&file) / sizeof(SaveListEntry);
    auto entries = std::make_unique_for_overwrite<SaveListEntry[]>(entryCount);
    UINT bytesRead = 0;
    FRESULT result = f_read(&file, entries.get(), entryCount * sizeof(SaveListEntry), &bytesRead);
    if (result != FR_OK || bytesRead != entryCount * sizeof(SaveListEntry))
    {
        LOG_FATAL("Failed to read save list file\n");
        return nullptr;
    }
    f_close(&file);

    return std::make_unique<SaveList>(std::move(entries), entryCount);
}