#pragma once
#include <array>
#include <memory>
#include "core/task/TaskQueue.h"
#include "cheats/GameCheats.h"
#include "romBrowser/FileInfo.h"
#include "romBrowser/IRomBrowserController.h"

/// @brief View model for the cheats screen.
class CheatsViewModel
{
public:
    enum class State
    {
        Loading,
        NoCheats,
        DisplayCheats
    };

    CheatsViewModel(const FileInfo& romFileInfo, IRomBrowserController* romBrowserController);

    void ItemActivated();
    void Back();
    void Close();
    void DisableAllCheats();

    State GetState() const { return _state; }
    const ICheatCategory* GetCurrentCheatCategory() const { return _categoryStack[_categoryStackLevel].cheatCategory; }

    constexpr int GetSelectedItem() const { return _selectedItem; }
    void SetSelectedItem(int selectedItem) { _selectedItem = selectedItem; }

private:
    struct CategoryStackEntry
    {
        const ICheatCategory* cheatCategory;
        u32 index;
    };

    FileInfo _romFileInfo;
    IRomBrowserController* _romBrowserController;
    QueueTask<void> _loadCheatsTask;
    std::unique_ptr<GameCheats> _cheats;
    State _state = State::Loading;
    int _selectedItem = -1;
    bool _changed = false;
    u32 _categoryStackLevel = 0;
    std::array<CategoryStackEntry, 8> _categoryStack;

    void DisableAllCheats(const ICheatCategory* cheatCategory);
};
