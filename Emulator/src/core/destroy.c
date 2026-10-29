#include "destroy.h"

void destroy(Emulator *emu){
    SDLite_DestroyInput(emu->SDLite_io.input);
    SDLite_DestroyTime(emu->SDLite_io.time);
    t_free(emu);
}

void destroy_render_data(RenderData *render_data) {
    if(render_data->emu_texture) SDL_DestroyTexture(render_data->emu_texture);
    if(render_data->palette) SDL_DestroyPalette(render_data->palette);
    if(render_data->emu_hz) SDLite_DestroyText(render_data->emu_hz);
    if(render_data->font) SDLite_Destroy_Text_Font(&render_data->font);
    if(render_data->display) SDLite_DestroyDisplay(render_data->display);
}

void destroy_shared_data(SharedData *shared_data) {
    if(shared_data->vram_zero_m) SDL_DestroyMutex(shared_data->vram_zero_m);
    if(shared_data->vram_one_m) SDL_DestroyMutex(shared_data->vram_one_m);
}