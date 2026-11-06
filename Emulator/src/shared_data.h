#ifndef EMU_SHARED_DATA_H
#define EMU_SHARED_DATA_H

#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_atomic.h>
#include "my_stdtypes.h"

#define EMU_DISPLAY_SIZE 64
#define VRAM_SIZE EMU_DISPLAY_SIZE * EMU_DISPLAY_SIZE


typedef enum {

    DSPCFG_UNSIGNED_WORD,
    DSPCFG_SIGNED_WORD,
    DSPCFG_FLOAT,

} NumDisplayConfig;


typedef struct {

    SDL_AtomicU32 signal_safe_quit, signal_fail_quit, signal_test, signal_change_hz;
    SDL_AtomicU32 hz_setpoint; 

} UserInput;


typedef struct {

    UserInput input;
    u8 *vram_zero;
    u8 *vram_one;
    SDL_Mutex *vram_zero_m;
    SDL_Mutex *vram_one_m;
    SDL_AtomicU32 emu_hertz;
    SDL_AtomicU32 signal_emu_active_vram_bank;
    SDL_AtomicU32 signal_emu_done;
    
} SharedData;

#endif