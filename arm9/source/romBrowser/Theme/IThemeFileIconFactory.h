#pragma once

class FileIcon;

class IThemeFileIconFactory
{
public:
    virtual ~IThemeFileIconFactory() = 0;
    virtual std::unique_ptr<FileIcon> CreateFolderIcon(const TCHAR* name) const = 0;
    virtual std::unique_ptr<FileIcon> CreateGenericFileIcon(const TCHAR* name) const = 0;
    virtual std::unique_ptr<FileIcon> CreateNdsFileIcon(const TCHAR* name) const = 0;
    virtual std::unique_ptr<FileIcon> CreateThemeFileIcon() const = 0;
};

inline IThemeFileIconFactory::~IThemeFileIconFactory() { }
