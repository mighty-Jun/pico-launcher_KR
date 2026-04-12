#pragma once
#include "common.h"
#include "../IRomBrowserController.h"

class LoadingViewModel
{
public:
    LoadingViewModel(IRomBrowserController* romBrowserController);

private:
    IRomBrowserController* _romBrowserController;
};