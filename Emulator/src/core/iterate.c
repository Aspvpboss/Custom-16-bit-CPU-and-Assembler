#include "iterate.h"

#include "pipeline/execute.h"
#include "pipeline/fetch.h"
#include "pipeline/decode.h"
#include "pipeline/mmio.h"
#include "core/test.h"

EMU_Result iterate(Emulator *emu){

    SDLite_Input *input = emu->SDLite_io.input;
    if(SDL_GetAtomicU32(&emu->shared_data->input.signal_safe_quit)){
        return EMU_SUCCESS;
    }
   
    if(SDL_GetAtomicU32(&emu->shared_data->input.signal_fail_quit)){
        SDLite_SetError("iterate(): forced failure triggered via F key");
        return EMU_FAILURE;
    }

    if(SDL_GetAtomicU32(&emu->shared_data->input.signal_test)){
        write_mmio(emu, 0x7F09, 0, false);
        SDL_SetAtomicU32(&emu->shared_data->input.signal_test, 0);
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
    
    return EMU_CONTINUE;
}