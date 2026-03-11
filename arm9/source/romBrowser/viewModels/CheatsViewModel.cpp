#include "common.h"
#include "cheats/ICheatRepository.h"
#include "fat/File.h"
#include "CheatsViewModel.h"

CheatsViewModel::CheatsViewModel(const FileInfo& romFileInfo, IRomBrowserController* romBrowserController)
    : _romFileInfo(romFileInfo), _romBrowserController(romBrowserController)
{
    _categoryStack.fill({ nullptr, 0 });
    _loadCheatsTask = _romBrowserController->GetIoTaskQueue()->Enqueue([this] (const vu8& cancelRequested)
    {
        _cheats = _romBrowserController->GetCheatRepository().GetCheatsForGame(_romFileInfo.GetFastFileRef());
        if (_cheats)
        {
            _categoryStack[0] = { _cheats.get(), 0 };
            _state = State::DisplayCheats;
        }
        else
        {
            _state = State::NoCheats;
        }

        return TaskResult<void>::Completed();
    });
}

void CheatsViewModel::ActivateSelectedItem()
{
    auto cheatCategory = GetCurrentCheatCategory();
    u32 numberOfCategories = 0;
    auto categories = cheatCategory->GetCategories(numberOfCategories);
    u32 numberOfCheats = 0;
    auto cheats = cheatCategory->GetCheats(numberOfCheats);

    if (_selectedItem < (int)numberOfCategories)
    {
        // Category activated
        if (_categoryStackLevel + 1 != _categoryStack.size())
        {
            _categoryStack[++_categoryStackLevel] = { &categories[_selectedItem], (u32)_selectedItem };
            _selectedItem = 0;
        }
    }
    else
    {
        // Toggle cheat on/off
        auto& cheat = cheats[_selectedItem - numberOfCategories];
        bool isEnabled = !cheat.GetIsCheatActive();
        if (isEnabled && cheatCategory->GetIsMaxOneCheatActive())
        {
            for (u32 i = 0; i < numberOfCheats; i++)
            {
                cheats[i].SetIsCheatActive(false);
            }
        }
        cheat.SetIsCheatActive(isEnabled);
        _changed = true;
    }
}

bool CheatsViewModel::NavigateUp()
{
    if (_categoryStackLevel == 0)
    {
        Close();
        return false;
    }
    else
    {
        _selectedItem = _categoryStack[_categoryStackLevel].index;
        _categoryStack[_categoryStackLevel--] = { nullptr, 0 };
        return true;
    }
}

void CheatsViewModel::Close()
{
    _categoryStack.fill({ nullptr, 0 });

    if (_changed)
    {
        // Save which cheats are enabled/disabled
        _romBrowserController->GetIoTaskQueue()->Enqueue(
            [romBrowserController = _romBrowserController, cheats = move(_cheats)] (const vu8& cancelRequested)
            {
                romBrowserController->GetCheatRepository().UpdateEnabledCheatsForGame(cheats);
                return TaskResult<void>::Completed();
            });
    }

    _romBrowserController->HideGameInfo();
}

void CheatsViewModel::DisableAllCheats()
{
    DisableAllCheats(_cheats.get());
}

void CheatsViewModel::DisableAllCheats(const ICheatCategory* cheatCategory)
{
    u32 numberOfCategories = 0;
    auto categories = cheatCategory->GetCategories(numberOfCategories);
    for (u32 i = 0; i < numberOfCategories; i++)
    {
        DisableAllCheats(&categories[i]);
    }

    u32 numberOfCheats = 0;
    auto cheats = cheatCategory->GetCheats(numberOfCheats);
    for (u32 i = 0; i < numberOfCheats; i++)
    {
        if (cheats[i].GetIsCheatActive())
        {
            cheats[i].SetIsCheatActive(false);
            _changed = true;
        }
    }
}
