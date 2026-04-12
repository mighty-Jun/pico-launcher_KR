#include "common.h"
#include <algorithm>
#include <string.h>
#include "SaveList.h"
#include "SaveListFactory.h"
#include "fileInfo.h"
#include "gameCode.h"
#include "CardSaveArranger.h"
#include "ndsHeader.h"

#define SAVE_LIST_PATH      "/_pico/savelist.bin"
#define DEFAULT_SAVE_SIZE   (512 * 1024)
#define SAVE_FILL_VALUE     0xFF
#define NTR_NAND_BLOCK_SIZE 0x20000
#define TWL_NAND_BLOCK_SIZE 0x80000
#define NAND_RW_REGION_END  0x07A00000

static const u8 sBandBrothersSaveId[12] = { 0x48, 0x8A, 0x00, 0x00, 0x42, 0x42, 0x44, 0x58, 0x31, 0x32, 0x33, 0x34 };
static const u8 sJamWithTheBandSaveId[16] = { 0xEC, 0x00, 0x9E, 0xA1, 0x51, 0x65, 0x34, 0x35, 0x30, 0x35, 0x30, 0x31, 0x19, 0x19, 0x02, 0x0A };

bool CardSaveArranger::SetupCardSave(const TCHAR* romPath, const TCHAR* savePath) const
{
    LOG_DEBUG("SAVE_DEBUG: 1. SetupCardSave Start");

    static nds_header_ntr_t header;
    static FIL romFile;
    static FIL file;
    static u8 ffBuffer[4096];
    UINT bytesRead;

    if (f_open(&romFile, romPath, FA_READ | FA_OPEN_EXISTING) != FR_OK) return false;
    f_read(&romFile, &header, sizeof(nds_header_ntr_t), &bytesRead);
    f_close(&romFile);
    
    LOG_DEBUG("SAVE_DEBUG: 2. Header Read OK");

    u32 saveSize = DEFAULT_SAVE_SIZE;
    if (header.nandBackupRegionStart != 0)
    {
        u32 blockSize = header.SupportsDsiMode() ? TWL_NAND_BLOCK_SIZE : NTR_NAND_BLOCK_SIZE;
        saveSize = NAND_RW_REGION_END - (header.nandBackupRegionStart * blockSize);
        LOG_DEBUG("SAVE_DEBUG: 3A. NAND Size Calculated");
    }
    else
    {
        LOG_DEBUG("SAVE_DEBUG: 3B. Loading SaveList...");
        auto saveList = SaveListFactory().CreateFromFile(SAVE_LIST_PATH);
        LOG_DEBUG("SAVE_DEBUG: 3C. SaveList Loaded");
        
        if (saveList) {
            const auto saveListEntry = saveList->FindEntry(header.gameCode);
            if (saveListEntry) saveSize = saveListEntry->GetSaveSize();
        }
    }

    if (saveSize == 0) return true;

    LOG_DEBUG("SAVE_DEBUG: 4. Creating .sav file...");
    if (f_open(&file, savePath, FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK) return false;

    u32 initialSize = f_size(&file);
    if (initialSize < saveSize)
    {
        LOG_DEBUG("SAVE_DEBUG: 5. Filling 0xFF...");
        f_lseek(&file, saveSize);
        f_lseek(&file, initialSize);
        memset(ffBuffer, SAVE_FILL_VALUE, sizeof(ffBuffer));

        u32 offset = initialSize;
        while (offset < saveSize)
        {
            u32 bytesToWrite = std::min<u32>(saveSize - offset, sizeof(ffBuffer));
            UINT written = 0;
            f_write(&file, ffBuffer, bytesToWrite, &written);
            offset += bytesToWrite;
        }
        LOG_DEBUG("SAVE_DEBUG: 6. Fill Complete");
    }

    f_close(&file);
    LOG_DEBUG("SAVE_DEBUG: 7. SetupCardSave Done!");
    return true;
}