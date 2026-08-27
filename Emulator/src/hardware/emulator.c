#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "hardware/emulator.h"

#define RETURN_THREAD(status) SDL_SetAtomicU32(emulator_state, 1); return status; 

int emulator_main(void *ptr){
  
    SDL_AtomicU32 *emulator_state = (SDL_AtomicU32*)ptr;

    if(RUN_TESTS()){
        RETURN_THREAD(1);
    }

    Emulator *emu = init();
    if(!emu){
        RETURN_THREAD(1);
    }


    EMU_Result result = EMU_CONTINUE;
    SDLite_IO *sdlite = &emu->SDLite_io;

    bool limit_fps = false;

    while(result == EMU_CONTINUE){

        result = iterate(emu);
        
        if(result == EMU_FAILURE){
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