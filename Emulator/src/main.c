#include <SDLite/SDLite.h>
#include <SDL3/SDL_thread.h>
#include "hardware/emulator.h"
#include "core/init.h"
#include "core/destroy.h"
#include "core/render_update.h"


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

    RenderData render = {0};
    SharedData shared_data = {0};
    SDL_SetAtomicU32(&shared_data.signal_emu_active_vram_bank, 0);
    SDL_SetAtomicU32(&shared_data.signal_emu_done, 0);
    SDL_SetAtomicU32(&shared_data.emu_hertz, 0);

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
 
    if(check_version())
        return 1;

    if(create_render_data(&render)){
        destroy_render_data(&render);
        SDLite_Quit();
        return 1;
    }

    if(!render.display){
        SDLite_Quit();
        return 1;
    }

    shared_data.vram_zero_m = SDL_CreateMutex();
    shared_data.vram_one_m = SDL_CreateMutex();
    if(!shared_data.vram_zero_m || !shared_data.vram_one_m){
        destroy_shared_data(&shared_data);
        destroy_render_data(&render);
        SDLite_Quit();
        return 1;
    }

    SDL_Thread *emulator_thread = SDL_CreateThread(emulator_main, "emulator", &shared_data);
    if(!emulator_thread){
        destroy_shared_data(&shared_data);
        destroy_render_data(&render);
        SDLite_Quit();
        return 1;
    }

    while((!SDL_GetAtomicPointer((void**)&shared_data.vram_zero) || !SDL_GetAtomicPointer((void**)&shared_data.vram_one)) &&
          !SDL_GetAtomicU32(&shared_data.signal_emu_done)){
        SDL_Delay(1);
    }


    SDL_Renderer *sdl_renderer = SDLite_Display_GetSDLRenderer(render.display);
    while(!SDL_GetAtomicU32(&shared_data.signal_emu_done)){

        static SDL_Event e;

        while(SDL_PollEvent(&e)){
            // nothing
        }

        u32 current_hertz = SDL_GetAtomicU32(&shared_data.emu_hertz);
        update_emu_hz_text(render.emu_hz, current_hertz);
        update_render_texture(&shared_data, &render);

        // main rendering loop
        SDLite_DisplayClear(render.display);
        
        SDLite_Render_Text(render.emu_hz);
        SDL_RenderTexture(sdl_renderer, render.emu_texture, NULL, &render.render_rect);
        
        SDLite_DisplayPresent(render.display);
        
    }


    int emulator_result = 0;
    SDL_WaitThread(emulator_thread, &emulator_result);

    destroy_shared_data(&shared_data);
    destroy_render_data(&render);

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