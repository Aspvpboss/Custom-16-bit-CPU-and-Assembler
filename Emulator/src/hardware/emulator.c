#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"

#define RETURN_THREAD(status) SDL_SetAtomicU32(&shared_data->signal_emu_done, 1); return status; 
#define BATCH_SIZE 100
#define DEFAULT_HZ_LIMIT 1000.0
// #define LOCK_MUTEX(vram_bank) vram_bank ? SDL_LockMutex(emu->shared_data->vram_one_m) ? SDL_LockMutex(emu->shared_data->vram_zero_m)
// #define UNLOCK_MUTEX(vram_bank) vram_bank ? SDL_UnlockMutex(emu->shared_data->vram_one_m) ? SDL_UnlockMutex(emu->shared_data->vram_zero_m)

inline static void emu_lock_mutex(Emulator *emu) {
    emu->ram.vram_selector ? SDL_LockMutex(emu->shared_data->vram_one_m) : SDL_LockMutex(emu->shared_data->vram_zero_m);
}
inline static void emu_unlock_mutex(Emulator *emu) {
    emu->ram.vram_selector ? SDL_UnlockMutex(emu->shared_data->vram_one_m) : SDL_UnlockMutex(emu->shared_data->vram_zero_m);
}

int emulator_main(void *ptr){
  
    SharedData *shared_data = (SharedData*)ptr;

    if(RUN_TESTS()){
        SDLite_SetError("emulator_main(): unit tests failed");
        RETURN_THREAD(1);
    }
    
    Emulator *emu = init();
    if(!emu){
        SDLite_SetError("emulator_main(): init() failed");
        RETURN_THREAD(1);
    }
    emu->shared_data = shared_data;

    SDL_SetAtomicPointer((void**)&shared_data->vram_zero, emu->ram.vram_zero);
    SDL_SetAtomicPointer((void**)&shared_data->vram_one, emu->ram.vram_one);


    EMU_Result result = EMU_CONTINUE;
    SDLite_IO *sdlite = &emu->SDLite_io;

    bool limit_fps = false;
    SDLite_Time_Set_FPSLimit(sdlite->time, DEFAULT_HZ_LIMIT / BATCH_SIZE);

    while(result == EMU_CONTINUE){

        for(u32 i = 0; i < BATCH_SIZE; i++){
            result = iterate(emu);

            if(result == EMU_FAILURE){
                SDLite_SetError("emulator_main(): iterate() failed");
                destroy(emu);
                RETURN_THREAD(1);
            }

            if(result != EMU_CONTINUE){
                break;
            }
        }

        double current_hz = SDLite_Time_GetFPS(sdlite->time) * BATCH_SIZE;

        if(SDLite_Input_KeyJustPressed(sdlite->input, SDL_SCANCODE_T)){
            printf("                                        \r %.2f\r", current_hz);
        }

        if(SDLite_Input_KeyJustPressed(sdlite->input, SDL_SCANCODE_SPACE)){
            limit_fps = !limit_fps;
            double new_fps = limit_fps ? -1 : (DEFAULT_HZ_LIMIT / BATCH_SIZE);
            SDLite_Time_Set_FPSLimit(sdlite->time, new_fps);
        }

        SDL_SetAtomicU32(&shared_data->emu_hertz, (u32)current_hz);

        SDLite_Input_UpdateAllPrev(sdlite->input);
        SDLite_TimeFunctions(sdlite->time);
    }

    destroy(emu);

    RETURN_THREAD(0);
}