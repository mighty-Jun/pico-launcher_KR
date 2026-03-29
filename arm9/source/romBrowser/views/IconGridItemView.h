#pragma once
#include <memory>
#include "gui/views/View.h"
#include "../FileType/FileIcon.h"

class MaterialColorScheme;

class IconGridItemView : public View
{
public:
    class VramToken
    {
        u32 _vramOffset;
    public:
        VramToken()
            : _vramOffset(0) { }

        explicit VramToken(u32 offset)
            : _vramOffset(offset) { }

        constexpr u32 GetVramOffset() const { return _vramOffset; }
    };

    void InitVram(const VramContext& vramContext) override;
    void Update() override;

    void SetIcon(std::unique_ptr<FileIcon> icon)
    {
        _icon = std::move(icon);
        if (_icon)
        {
            _icon->SetVramAddress(_iconVram, _iconVramOffset);
        }
    }

    void UploadIconGraphics() const
    {
        if (_icon)
        {
            _icon->UploadGraphics();
        }
    }

    virtual void SetGraphics(const VramToken& vramToken) { }

protected:
    std::unique_ptr<FileIcon> _icon;
    vu16* _iconVram;
    u32 _iconVramOffset;

    IconGridItemView()
        : _icon(nullptr) { }
};