#include "iterate.h"

#include "pipeline/execute.h"
#include "pipeline/fetch.h"
#include "pipeline/decode.h"
#include "pipeline/mmio.h"
#include "core/test.h"

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
        SDLite_SetError("iterate(): forced failure triggered via F key");
        return EMU_FAILURE;
    }

    EMU_Decoded_Instruction instruction = {0};

    i8 result = 0;

    result = fetch(emu, &instruction);
    if(result == 1){
        SDLite_SetError("fetch() failed to run");
        return EMU_FAILURE;
    }     
    result = decode(emu, &instruction);
    if(result == 1){
        SDLite_SetError("decode() failed to run");
        return EMU_FAILURE;
    }     
    result = execute(emu, &instruction);
    if(result == 1){
        SDLite_SetError("execute() failed to run");
        return EMU_FAILURE;
    } 
    result = mmio();
    if(result == 1){
        SDLite_SetError("mmio() failed to run");
        return EMU_FAILURE; 
    } 
    return EMU_CONTINUE;
}