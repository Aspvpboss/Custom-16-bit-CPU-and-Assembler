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

    SharedData shared_data = {0};
    SDL_SetAtomicU32(&shared_data.signal_emu_active_vram_bank, 0);
    SDL_SetAtomicU32(&shared_data.signal_emu_done, 0);

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
 
    if(check_version())
        return 1;

    SDLite_Display *display = SDLite_CreateDisplay("Rose EMU", 400, 400, SDL_WINDOW_MAXIMIZED);
    if(!display){
        SDLite_Quit();
        return 1;
    }

    shared_data.vram_zero_m = SDL_CreateMutex();
    shared_data.vram_one_m = SDL_CreateMutex();
    if(!shared_data.vram_zero_m || !shared_data.vram_one_m){
        SDL_DestroyMutex(shared_data.vram_zero_m);
        SDL_DestroyMutex(shared_data.vram_one_m);
        SDLite_DestroyDisplay(display);
        SDLite_Quit();
        return 1;
    }

    SDL_Thread *emulator_thread = SDL_CreateThread(emulator_main, "emulator", &shared_data);
    if(!emulator_thread){
        SDL_DestroyMutex(shared_data.vram_zero_m);
        SDL_DestroyMutex(shared_data.vram_one_m);
        SDLite_DestroyDisplay(display);
        SDLite_Quit();
        return 1;
    }

    while((!SDL_GetAtomicPointer((void**)&shared_data.vram_zero) || !SDL_GetAtomicPointer((void**)&shared_data.vram_one)) &&
          !SDL_GetAtomicU32(&shared_data.signal_emu_done)){
        SDL_Delay(1);
    }

    while(!SDL_GetAtomicU32(&shared_data.signal_emu_done)){
        // main rendering loop
        SDLite_DisplayClear(display);
        SDLite_DisplayPresent(display);
    }


    int emulator_result = 0;
    SDL_WaitThread(emulator_thread, &emulator_result);

    SDL_DestroyMutex(shared_data.vram_zero_m);
    SDL_DestroyMutex(shared_data.vram_one_m);

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