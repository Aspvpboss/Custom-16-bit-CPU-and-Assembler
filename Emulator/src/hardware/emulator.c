#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"
#include <SDL3/SDL_timer.h>

#define RETURN_THREAD(status) SDL_SetAtomicU32(&shared_data->signal_emu_done, 1); return status; 
#define BATCH_SIZE 100
#define DEFAULT_FPS_LIMIT 1000.0

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

    SDL_SetAtomicPointer((void**)&shared_data->vram_zero, emu->ram.vram_zero);
    SDL_SetAtomicPointer((void**)&shared_data->vram_one, emu->ram.vram_one);


    EMU_Result result = EMU_CONTINUE;
    SDLite_IO *sdlite = &emu->SDLite_io;

    bool limit_fps = true;
    double fps_limit = DEFAULT_FPS_LIMIT;
    double current_hz = 0.0;

    while(result == EMU_CONTINUE){

        u64 start_time = SDL_GetTicksNS();
        u32 loops_done = 0;

        for(u32 i = 0; i < BATCH_SIZE; i++){
            result = iterate(emu);
            loops_done++;

            if(result == EMU_FAILURE){
                SDLite_SetError("emulator_main(): iterate() failed");
                destroy(emu);
                RETURN_THREAD(1);
            }

            if(result != EMU_CONTINUE){
                break;
            }
        }

        if(limit_fps && fps_limit > 0.0){
            u64 target_ns = (u64)(((double)loops_done / fps_limit) * 1e9);
            u64 elapsed_ns = SDL_GetTicksNS() - start_time;
            if(elapsed_ns < target_ns){
                SDL_DelayPrecise(target_ns - elapsed_ns);
            }
        }

        u64 end_time = SDL_GetTicksNS();
        u64 time_diff = end_time - start_time;
        if(time_diff > 0){
            current_hz = ((double)loops_done * 1e9) / (double)time_diff;
        }

        if(SDLite_Input_KeyJustPressed(sdlite->input, SDL_SCANCODE_T)){
            printf("                                        \r %.2f\r", current_hz);
        }

        if(SDLite_Input_KeyJustPressed(sdlite->input, SDL_SCANCODE_SPACE)){
            limit_fps = !limit_fps;
            fps_limit = limit_fps ? DEFAULT_FPS_LIMIT : -1.0;
        }

        SDL_SetAtomicU32(&shared_data->emu_hertz, (u32)current_hz);
        SDLite_Input_UpdateAllPrev(sdlite->input);
    }

    destroy(emu);

    RETURN_THREAD(0);
}