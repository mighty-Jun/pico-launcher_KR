#pragma once

// Korean patch addition: sleep mode while the lid is closed.
// Kept self-contained so it can be dropped once upstream supports lid sleep:
// delete lidSleep.h/lidSleep.cpp and the lidSleep include and calls in arm7/source/main.cpp.

/// @brief Creates the thread that puts the console to sleep.
void lidSleep_init();

/// @brief Checks the lid. Call once per frame while the launcher is idle.
void lidSleep_update();
