#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"
#include "pipeline/execute/execute_memory.h"

#define RETURN_THREAD(status) SDL_SetAtomicU32(&shared_data->signal_emu_done, 1); return status; 
#define BATCH_SIZE 100
#define DEFAULT_HZ_LIMIT 1000.0

inline static void emu_lock_mutex(Emulator *emu, bool vram_mutex) {
    vram_mutex ? SDL_LockMutex(emu->shared_data->vram_one_m) : SDL_LockMutex(emu->shared_data->vram_zero_m);
}
inline static void emu_unlock_mutex(Emulator *emu, bool vram_mutex) {
    vram_mutex ? SDL_UnlockMutex(emu->shared_data->vram_one_m) : SDL_UnlockMutex(emu->shared_data->vram_zero_m);
}

void flush_vram_buffer(Emulator *emu, u8 flush_val) {
    emu_lock_mutex(emu, emu->ram.vram_selector);
    const u16 vram_start = 0x6F00;
    EMU_Ram *ram = &emu->ram;
    for(u16 y = 0; y < EMU_DISPLAY_SIZE; y++){
        for(u16 x = 0; x < EMU_DISPLAY_SIZE; x++){
            write_memory(ram, vram_start + (y * EMU_DISPLAY_SIZE + x), flush_val, false);
        }
    }
    
    emu_unlock_mutex(emu, emu->ram.vram_selector);
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

    emu->ram.vram_selector = true;
    flush_vram_buffer(emu, 2);
    emu->ram.vram_selector = false;
    flush_vram_buffer(emu, 1);

    SDL_SetAtomicPointer((void**)&shared_data->vram_zero, emu->ram.vram_zero);
    SDL_SetAtomicPointer((void**)&shared_data->vram_one, emu->ram.vram_one);


    EMU_Result result = EMU_CONTINUE;
    SDLite_IO *sdlite = &emu->SDLite_io;

    bool limit_fps = false;
    SDLite_Time_Set_FPSLimit(sdlite->time, DEFAULT_HZ_LIMIT / BATCH_SIZE);

    while(result == EMU_CONTINUE){

        for(u32 i = 0; i < BATCH_SIZE; i++){
            const bool current_emu_mutex = emu->ram.vram_selector;
            emu_lock_mutex(emu, current_emu_mutex);
            result = iterate(emu);
            emu_unlock_mutex(emu, current_emu_mutex);

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