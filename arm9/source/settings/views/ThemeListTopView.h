#pragma once
#include "core/SharedPtr.h"
#include "gui/views/Label2DView.h"
#include "gui/views/ViewContainer.h"
#include "settings/viewModels/ThemeListViewModel.h"

class IFontRepository;
class MaterialColorScheme;

class ThemeListTopView : public ViewContainer
{
    SHARED_ONLY(ThemeListTopView)

public:
    ThemeListTopView(SharedPtr<ThemeListViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository);

    void VBlank() override;

    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

private:
    SharedPtr<ThemeListViewModel> _viewModel;
    int _lastSelectedItem = -1;
    SharedPtr<Label2DView> _noPreviewLabel;
};
