#include "SDLite.h"
#include "emulator/emulator.h"



int main(){

    _Atomic int emulator_state = 0;  

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
    
    SDL_Thread *emulator_thread = SDL_CreateThread(emulator_main, "emulator", &emulator_state);
    SDL_DetachThread(emulator_thread);

    while(emulator_state == 0){
        printf(" main thread                  \r");
        SDL_Delay(4000);
    }

    if(emulator_state == 1){
        printf("emu sucesss: %d                  \n", emulator_state);
    } else{
        printf("emu failure %d                  \n", emulator_state);
    }

    SDLite_Quit();
    
    return 0;
}