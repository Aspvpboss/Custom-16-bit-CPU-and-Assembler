#include "iterate.h"

#include "pipeline/execute.h"
#include "pipeline/fetch.h"
#include "pipeline/decode.h"
#include "pipeline/mmio.h"


EMU_Result iterate(Emulator *emu){

    static SDL_Event e;

    while(SDL_PollEvent(&e)){
        if(e.type == SDL_EVENT_QUIT){
            return EMU_SUCCESS;
        }            
    }

    SDLite_Input *input = emu->SDLite_io.input;
    if(SDLite_Input_KeyPressed(input, SDL_SCANCODE_Q) && SDLite_Input_KeyPressed(input, SDL_SCANCODE_LSHIFT)){
        return EMU_SUCCESS;
    }
   
    if(SDLite_Input_KeyJustPressed(input, SDL_SCANCODE_F)){
        return EMU_FAILURE;
    }

    EMU_Decoded_Instruction instruction = {0};

    i8 result = 0;

    result = fetch(emu, &instruction);
    if(result == 1) return EMU_SUCCESS;
    
    result = decode(emu, &instruction);
    if(result == 1) return EMU_SUCCESS;
    
    result = execute(emu, &instruction);
    if(result == 1) return EMU_SUCCESS;

    result = mmio();
    if(result == 1) return EMU_SUCCESS; 

    return EMU_CONTINUE;
}