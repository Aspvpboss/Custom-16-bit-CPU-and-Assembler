#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"

#define RETURN_THREAD(status) SDL_SetAtomicU32(&shared_data->signal_emu_done, 1); return status; 

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

    bool limit_fps = false;

    while(result == EMU_CONTINUE){

        result = iterate(emu);
        
        if(result == EMU_FAILURE){
            SDLite_SetError("emulator_main(): iterate() failed");
            destroy(emu);
            RETURN_THREAD(1);
        }

        if(SDLite_Input_KeyJustPressed(emu->SDLite_io.input, SDL_SCANCODE_T)){
            printf("                                        \r %.2f - dt: %.10f\r", 
            SDLite_Time_GetFPS(sdlite->time), SDLite_Time_GetDT(sdlite->time));
        }

        if(SDLite_Input_KeyJustPressed(sdlite->input, SDL_SCANCODE_SPACE)){
            limit_fps = !limit_fps;
            double new_fps = limit_fps ? -1 : 1000;
            SDLite_Time_Set_FPSLimit(sdlite->time, new_fps);
        }


        SDLite_Input_UpdateAllPrev(emu->SDLite_io.input);
        SDLite_TimeFunctions(sdlite->time);
    }

    destroy(emu);

    RETURN_THREAD(0);
}