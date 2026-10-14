#pragma once
#include "../IThemeFileIconFactory.h"
#include "MaterialFolderIcon.h"
#include "MaterialGenericFileIcon.h"
#include "MaterialNdsFileIcon.h"
#include "MaterialThemeFileIcon.h"

class MaterialColorScheme;
class IFontRepository;

class MaterialThemeFileIconFactory : public IThemeFileIconFactory
{
public:
    explicit MaterialThemeFileIconFactory(const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository)
        : _materialColorScheme(materialColorScheme), _fontRepository(fontRepository) { }

    std::unique_ptr<FileIcon> CreateFolderIcon(const TCHAR* name) const override
    {
        return std::make_unique<MaterialFolderIcon>(name, _materialColorScheme, _fontRepository);
    }

    std::unique_ptr<FileIcon> CreateGenericFileIcon(const TCHAR* name) const override
    {
        return std::make_unique<MaterialGenericFileIcon>(name, _materialColorScheme, _fontRepository);
    }

    std::unique_ptr<FileIcon> CreateNdsFileIcon(const TCHAR* name) const override
    {
        return std::make_unique<MaterialNdsFileIcon>(name, _materialColorScheme, _fontRepository);
    }

    std::unique_ptr<FileIcon> CreateThemeFileIcon() const override
    {
        return std::make_unique<MaterialThemeFileIcon>(_materialColorScheme, _fontRepository);
    }

private:
    const MaterialColorScheme* _materialColorScheme;
    const IFontRepository* _fontRepository;
};
