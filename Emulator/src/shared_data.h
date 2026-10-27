#ifndef EMU_SHARED_DATA_H
#define EMU_SHARED_DATA_H

#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_atomic.h>
#include "my_stdtypes.h"

#define VRAM_SIZE 0x1000

typedef struct {

    u8 *vram_zero;
    u8 *vram_one;
    SDL_Mutex *vram_zero_m;
    SDL_Mutex *vram_one_m;
    SDL_AtomicU32 signal_emu_active_vram_bank;
    SDL_AtomicU32 signal_emu_done;

} SharedData;

#endif