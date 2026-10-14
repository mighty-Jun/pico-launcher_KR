#pragma once

#define SHARED_KEY_XY           (*(vu16*)0x02FFFFA8)
#define SHARED_TOUCH_X          (*(vu16*)0x02FFFFAA)
#define SHARED_TOUCH_Y          (*(vu16*)0x02FFFFAC)

// Battery state from libnds getBatteryLevel(), refreshed by arm7 about once a second
#define SHARED_BATTERY_LEVEL    (*(vu32*)0x02FFFFB0)
#define SHARED_BATTERY_LEVEL_MASK           0xF
#define SHARED_BATTERY_CHARGER_CONNECTED    (1 << 7)
#define SHARED_IS_3DS_FLAG      (*(vu8*)0x02FFFFB4)
