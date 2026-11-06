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


int render_loop(SharedData *shared_data, RenderData *render_data) {

    SDL_Renderer *sdl_renderer = SDLite_Display_GetSDLRenderer(render_data->display);
    while(!SDL_GetAtomicU32(&shared_data->signal_emu_done)){

        static SDL_Event e;

        while(SDL_PollEvent(&e)){
            if(e.type == SDL_EVENT_QUIT){
                SDL_SetAtomicU32(&shared_data->input.signal_safe_quit, 1);
                return 0;
            }
        }
        if(SDLite_Input_KeyPressed(render_data->input, SDL_SCANCODE_Q) && SDLite_Input_KeyPressed(render_data->input, SDL_SCANCODE_LSHIFT)){
            SDL_SetAtomicU32(&shared_data->input.signal_safe_quit, 1);
            return 0;
        }
       
        if(SDLite_Input_KeyJustPressed(render_data->input, SDL_SCANCODE_F)){
            SDLite_SetError("iterate(): forced failure triggered via F key");
            SDL_SetAtomicU32(&shared_data->input.signal_fail_quit, 1);
            return 1;
        }

        if(SDLite_Input_KeyJustPressed(render_data->input, SDL_SCANCODE_T)){
            SDL_SetAtomicU32(&shared_data->input.signal_test, 1);
        }

        if(SDLite_Input_KeyJustPressed(render_data->input, SDL_SCANCODE_SPACE)){
            SDL_SetAtomicU32(&shared_data->input.signal_change_hz, 1);
        }

        u32 current_hertz = SDL_GetAtomicU32(&shared_data->emu_hertz);
        update_emu_hz_text(render_data->emu_hz, current_hertz + 1);
        update_render_texture(shared_data, render_data);

        // main rendering loop
        SDLite_DisplayClear(render_data->display);
        
        SDLite_Render_Text(render_data->emu_hz);
        SDL_RenderTexture(sdl_renderer, render_data->emu_texture, NULL, &render_data->render_rect);
        
        SDLite_DisplayPresent(render_data->display);
   
        SDLite_Input_UpdateAllPrev(render_data->input);
        SDLite_TimeFunctions(render_data->time);
    }
    
    return 0;
}


int main(){

    RenderData render = {0};
    SharedData shared_data = {0};

    if(SDLite_Init(NULL, NULL, false, false))
        return 1;
 
    if(check_version())
        return 1;

    if(create_render_data(&render)){
        destroy_render_data(&render);
        SDLite_Quit();
        return 1;
    }

    if(create_shared_data(&shared_data)){
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


    if(render_loop(&shared_data, &render)){
        // fail stuff
    };

    int emulator_result = 0;
    SDL_WaitThread(emulator_thread, &emulator_result);

    destroy_shared_data(&shared_data);
    destroy_render_data(&render);

    const char *err = NULL;

    while((err = SDLite_GetError())){
        fprintf(stderr, "%s\n", err);
    }

    SDLite_Quit();
    
    return 0;
}