#pragma once

#define SHARED_KEY_XY           (*(vu16*)0x02FFFFA8)

// ★ 추가: ARM7과 ARM9이 공유할 32비트 배터리 우체통 (주소를 4바이트 띄움)
#define SHARED_BATTERY_LEVEL    (*(vu32*)0x02FFFFAC)