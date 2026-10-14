#pragma once
#include <memory>
#include "FileInfo.h"
#include "SdFolderFilterSortParams.h"

/// @brief Class representing the contents of a folder on the sd card.
class SdFolder
{
    FileInfo** _files;
    int _fileCount;

    static bool CompareClassification(const FileInfo* a, const FileInfo* b, bool& result);
    static bool CompareName(const FileInfo* a, const FileInfo* b);
public:
    /// @brief Creates an SdFolder from the given files. The ownership of both the array
    ///        and the FileInfo structs it points to will be taken over.
    /// @param files The array of files.
    /// @param fileCount The number of files in the array.
    SdFolder(FileInfo** files, int fileCount);

    /// @brief Destructs the folder, together with the FileInfo structs.
    ~SdFolder();

    /// @brief Filters and sorts the files in this folder using the given filterSortParams.
    /// @param filterSortParams The filter and sort parameters.
    /// @param resultCount Will contain the number of results.
    /// @return A unique pointer to an array of FileInfo pointers containing the results.
    std::unique_ptr<const FileInfo*[]> FilterAndSort(
        const SdFolderFilterSortParams& filterSortParams, u32& resultCount) const;

    void SortByNameInPlace();
    const FileInfo* BinarySearch(const char* fileName) const;

    /// @brief Gets a pointer to an array with all files in the folder.
    /// @return A pointer to an array with all files in the folder.
    const FileInfo* const* GetFiles() const { return _files; }

    /// @brief Gets the total number of files in the folder.
    /// @return The total number of files.
    int GetFileCount() const { return _fileCount; }
};