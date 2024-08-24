#pragma once

/*note@: change the VM machine frequently, use different ones on every 2 - 3 updates.
these virtual machines will keep changing within time.*/

// TIGER_RED - Complexity:21% | Speed:95%
// TIGER_BLACK - Complexity: 25% | Speed: 92%
// TIGER_WHITE - Complexity: 15% | Speed: 96%
// DOLPHIN_BLACK - Complexity: 38% | Speed: 45%
// DOLPHIN_RED - Complexity: 33% | Speed: 48%
// DOLPHIN_WHITE - Complexity: 26% | Speed: 86%
// EAGLE_BLACK - Complexity: 93% | Speed: 3%
// EAGLE_RED - Complexity: 84% | Speed: 5%
// EAGLE_WHITE - Complexity: 80% | Speed: 6%
// FISH_BLACK - Complexity: 37% | Speed: 5%
// FISH_RED - Complexity: 20% | Speed: 85%
// FISH_WHITE - Complexity: 10% | Speed: 90%
// PUMA_BLACK - Complexity: 89% | Speed: 12%
// PUMA_RED - Complexity: 87% | Speed: 17%
// PUMA_WHITE - Complexity: 80% | Speed: 16%
// SHARK - Complexity: 93% | Speed: 6%
// SHARK - Complexity: 85% | Speed: 20%
// SHARK - Complexity: 80% | Speed: 25%

#ifdef _PRODUCTION
#define VM_DIFF_START VM_DOLPHIN_BLACK_START
#define VM_DIFF_END VM_DOLPHIN_BLACK_END
#define VM_TIGER_START VM_TIGER_BLACK_START
#define VM_TIGER_END VM_TIGER_BLACK_END
#else 
#define VM_DIFF_START
#define VM_DIFF_END
#define VM_TIGER_START
#define VM_TIGER_END
#endif