#include "hardware/emulator.h"
#include <SDLite/SDLite.h>
#include <SDL3/SDL_thread.h>

#define SDL_WHITE (SDL_Color){255, 255, 255, 255}
#define FONT_PATH "assets/font.tff"
#define TOP_BAR_TEXT_HEIGHT 20

int check_version(){

    int macro, minor, micro;
    SDLite_Get_Version(&macro, &minor, &micro);
    if(!(macro == 1 && minor == 3 && micro == 0)){
        SDLite_SetError("Expected SDLite version to be v1.3.0");
        return 1;
    }

    return 0;
}
/*

    SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){

        SDK_Init(NULL, NULL, true, true, false);

        (*appstate) = t_malloc(sizeof(Appstate));
        Appstate *state = (Appstate*)(*appstate);

        state->display = SDK_CreateDisplay("SDK game", 1000, 800, SDL_WINDOW_MAXIMIZED);
        if(!state->display){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        state->time = SDK_CreateTime(144);
        if(!state->time){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        state->input = SDK_CreateInput();
        if(!state->input){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        state->fps_text = SDK_CreateText(state->display, NULL, 30, 0, 0, (SDL_Color){255, 255, 255, 255});
        if(!state->fps_text){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;        
        }

        int width = state->display->width;
        int height = state->display->height;

        state->frame_buffers[0] = t_malloc(sizeof(uint8_t) * (width * height));
        if(!state->frame_buffers[0]){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;        
        }
        state->frame_buffers[1] = t_malloc(sizeof(uint8_t) * (width * height));
        if(!state->frame_buffers[1]){
            SDL_Log("Error: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;        
        }

        state->palette = SDL_CreatePalette(256);
        SDL_Color colors[] = {
            {0, 0, 0, 0,},
            {255, 0, 0, 255},
            {0, 255, 0, 255}
        };

        SDL_SetPaletteColors(state->palette, colors, 0, sizeof(colors) / sizeof(SDL_Color));

        state->current_buffer = 0;

        state->texture = SDL_CreateTexture(
            state->display->renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, width, height);

        SDL_SetTexturePalette(state->texture, state->palette);

        for(int i = 0; i < 2; i++){
            flush_buffer(state->frame_buffers[i], width, height, i + 1);
        }


        return SDL_APP_CONTINUE;
    }


    int update(Appstate *state){

        SDK_Display *display = state->display;

        if(state->time->fps_updated)
            update_fps(state->fps_text, state->time);

        void *tex_pixels = NULL;
        int pitch = 0;

        if(!SDL_LockTexture(state->texture, NULL, &tex_pixels, &pitch)){
            SDL_Log("lock texture error: %s\n", SDL_GetError());
            return 1;
        }

        uint8_t *src = state->frame_buffers[state->current_buffer];
        uint8_t *dst = tex_pixels;

        for (int y = 0; y < display->height; y++) {
            memcpy(dst + y * pitch, src + y * display->width, display->width);
        }

        SDL_UnlockTexture(state->texture);
        
        return 0;
    }

*/

typedef struct {

    SDLite_Display *display;
    SDLite_Text *emu_hz;
    SDLite_Text_Font *font;
    u32 width, height;
    u8 scale;

} RenderData;

int create_render_data(RenderData *render_data) {
    render_data->scale = 1;
    render_data->height = EMU_DISPLAY_SIZE + TOP_BAR_TEXT_HEIGHT; 
    render_data->width = EMU_DISPLAY_SIZE;

    render_data->display = SDLite_CreateDisplay("Rose EMU", render_data->width, render_data->height, SDL_WINDOW_MAXIMIZED);
    if(!render_data->display) return 1;

    render_data->font = SDLite_Create_Text_Font(FONT_PATH, TOP_BAR_TEXT_HEIGHT);
    if(!render_data->font) return 1;
    render_data->emu_hz = SDLite_CreateText(render_data->display, NULL, render_data->font, 0, 0, SDL_WHITE);
    if(!render_data->emu_hz) return 1;

    return 0;
}

void destroy_render_data(RenderData *render_data) {
    if(render_data->emu_hz) SDLite_DestroyText(render_data->emu_hz);
    if(render_data->font) SDLite_Destroy_Text_Font(&render_data->font);
    if(render_data->display) SDLite_DestroyDisplay(render_data->display);
    
}

void destroy_shared_data(SharedData *shared_data) {
    if(shared_data->vram_zero_m) SDL_DestroyMutex(shared_data->vram_zero_m);
    if(shared_data->vram_one_m) SDL_DestroyMutex(shared_data->vram_one_m);
}

int main(){

    RenderData render = {0};
    SharedData shared_data = {0};
    SDL_SetAtomicU32(&shared_data.signal_emu_active_vram_bank, 0);
    SDL_SetAtomicU32(&shared_data.signal_emu_done, 0);

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

    while(!SDL_GetAtomicU32(&shared_data.signal_emu_done)){

        int active_vram_bank = SDL_GetAtomicU32(&shared_data.signal_emu_active_vram_bank);
        if(active_vram_bank == 0){
            SDL_LockMutex(shared_data.vram_zero_m);
        } else {
            SDL_LockMutex(shared_data.vram_one_m);
        }     

        // main rendering loop
        SDLite_DisplayClear(render.display);
        
        
        
        SDLite_DisplayPresent(render.display);
        
        if(active_vram_bank == 0){
            SDL_UnlockMutex(shared_data.vram_zero_m);
        } else {
            SDL_UnlockMutex(shared_data.vram_one_m);
        }     
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