#pragma once
#include <memory>
#include "core/AtomicSharedPtr.h"
#include "gui/views/View.h"
#include "../FileType/FileCover.h"
#include "gui/VBlankTextureLoader.h"

#define COVER_THICKNESS     (fix32<12>(0.23).GetRawValue())

class CoverView : public View
{
public:
    explicit CoverView(VBlankTextureLoader* vblankTextureLoader)
        : _vblankTextureLoader(vblankTextureLoader) { }

    ~CoverView() override
    {
        _vblankTextureLoader->CancelLoad(_textureLoadRequest);
    }

    void InitVram(const VramContext& vramContext) override;

    void Draw(GraphicsContext& graphicsContext) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position.x - (COVER_WIDTH / 2), _position.y - (COVER_HEIGHT / 2), COVER_WIDTH, COVER_HEIGHT);
    }

    void SetCover(SharedPtr<FileCover> cover)
    {
        _vblankTextureLoader->CancelLoad(_textureLoadRequest);
        _cover = std::move(cover);
    }

    void ClearCover()
    {
        _vblankTextureLoader->CancelLoad(_textureLoadRequest);
        _cover.Reset();
    }

    void UploadCoverGraphics();

private:
    VBlankTextureLoader* _vblankTextureLoader;
    AtomicSharedPtr<FileCover> _cover;
    VBlankTextureLoadRequest _textureLoadRequest;
    u32 _texVramOffset = 0;
    u32 _plttVramOffset = 0;
};
