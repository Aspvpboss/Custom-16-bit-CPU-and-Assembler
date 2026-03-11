#include "SDLite.h"
#include "emulator/emulator.h"



int main(){

    SDL_AtomicU32 emulator_state;
    SDL_SetAtomicU32(&emulator_state, 0);

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
    
    SDL_Thread *emulator_thread = SDL_CreateThread(emulator_main, "emulator", &emulator_state);
    if(!emulator_thread){
        SDLite_Quit();
        return 1;
    }

    while(!SDL_GetAtomicU32(&emulator_state)){
        printf(" main thread                  \r");
        SDL_Delay(4000);
    }


    int emulator_result = 0;
    SDL_WaitThread(emulator_thread, &emulator_result);

    if(emulator_result == 0){
        printf("emu sucesss: %d                  \n", emulator_result);
    } else{
        printf("emu failure %d                  \n", emulator_result);
    }

    SDLite_Quit();
    
    return 0;
}