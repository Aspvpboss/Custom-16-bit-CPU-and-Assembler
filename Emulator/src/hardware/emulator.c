#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"
#include "pipeline/execute/execute_memory.h"

#define RETURN_THREAD(status) SDL_SetAtomicU32(&shared_data->signal_emu_done, 1); return status; 
#define BATCH_SIZE 100
#define DEFAULT_HZ_LIMIT 1.0

void flush_vram_buffer(Emulator *emu, u8 flush_val) {
    const u16 vram_start = 0x6F00;
    for(u16 y = 0; y < EMU_DISPLAY_SIZE; y++){
        for(u16 x = 0; x < EMU_DISPLAY_SIZE; x++){
            write_memory(emu, vram_start + (y * EMU_DISPLAY_SIZE + x), flush_val, false);
        }
    }
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

    u64 batch_size = DEFAULT_HZ_LIMIT / 1000;
    if(batch_size == 0) batch_size = 1;
    bool limit_fps = false;
    SDLite_Time_Set_FPSLimit(sdlite->time, DEFAULT_HZ_LIMIT / batch_size);

    while(result == EMU_CONTINUE){

        for(u32 i = 0; i < batch_size; i++){
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

        if(SDL_GetAtomicU32(&shared_data->input.signal_change_hz)){
            limit_fps = !limit_fps;
            double new_fps;
            if(limit_fps){
                new_fps = -1;
                batch_size = 2000;
            } else {
                batch_size = DEFAULT_HZ_LIMIT / 1000;
                if(batch_size == 0) batch_size = 1;
                new_fps = (DEFAULT_HZ_LIMIT / batch_size);
            }
            SDLite_Time_Set_FPSLimit(sdlite->time, new_fps);
            SDL_SetAtomicU32(&shared_data->input.signal_change_hz, 0);
        }

        double current_hz = SDLite_Time_GetFPS(sdlite->time) * batch_size;
        SDL_SetAtomicU32(&shared_data->emu_hertz, (u32)current_hz);

        SDLite_TimeFunctions(sdlite->time);
    }

    destroy(emu);

    RETURN_THREAD(0);
}