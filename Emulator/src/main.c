#include "hardware/emulator.h"
#include <SDLite/SDLite.h>
#include <SDL3/SDL_thread.h>


int check_version(){

    int macro, minor, micro;
    SDLite_Get_Version(&macro, &minor, &micro);
    if(!(macro == 1 && minor == 3 && micro == 0)){
        SDLite_SetError("Expected SDLite version to be v1.3.0");
        return 1;
    }

    return 0;
}

int main(){

    SDL_AtomicU32 emulator_state;
    SDL_SetAtomicU32(&emulator_state, 0);

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
 
    if(check_version())
        return 1;

    SDL_Thread *emulator_thread = SDL_CreateThread(emulator_main, "emulator", &emulator_state);
    if(!emulator_thread){
        SDLite_Quit();
        return 1;
    }

    while(!SDL_GetAtomicU32(&emulator_state)){

        // main rendering loop
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


    const char *err = NULL;

    while((err = SDLite_GetError())){
        fprintf(stderr, "%s\n", err);
    }

    SDLite_Quit();
    
    return 0;
}