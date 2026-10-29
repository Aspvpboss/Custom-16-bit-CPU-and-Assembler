#include "core/render_update.h"


void update_render_texture(SharedData *shared_data, RenderData *render){
    int active_vram_bank = SDL_GetAtomicU32(&shared_data->signal_emu_active_vram_bank);
    if(active_vram_bank == 0){
        SDL_LockMutex(shared_data->vram_zero_m);
    } else {
        SDL_LockMutex(shared_data->vram_one_m);
    }     

    u8 *src = active_vram_bank ? shared_data->vram_one : shared_data->vram_zero;

    i32 pitch = 0; 
    void *dst = NULL;
    if(!SDL_LockTexture(render->emu_texture, NULL, &dst, &pitch)){
        if(active_vram_bank == 0){
            SDL_UnlockMutex(shared_data->vram_zero_m);
        } else {
            SDL_UnlockMutex(shared_data->vram_one_m);
        }     
        return;
    }

    u8 scale = render->scale;
    u8 *dst_pixels = (u8 *)dst;
    for (int y = 0; y < EMU_DISPLAY_SIZE; y++) {
        u8 *first_scaled_row = dst_pixels + (y * scale) * pitch;
        for (int x = 0; x < EMU_DISPLAY_SIZE; x++) {
            memset(first_scaled_row + x * scale, src[y * EMU_DISPLAY_SIZE + x], scale);
        }
        for (int sy = 1; sy < scale; sy++) {
            memcpy(dst_pixels + (y * scale + sy) * pitch, first_scaled_row, EMU_DISPLAY_SIZE * scale);
        }
    }

    SDL_UnlockTexture(render->emu_texture);

    if(active_vram_bank == 0){
        SDL_UnlockMutex(shared_data->vram_zero_m);
    } else {
        SDL_UnlockMutex(shared_data->vram_one_m);
    }     
}


void update_emu_hz_text(SDLite_Text *hz_text, u32 hz) {
    u64 current_ms = SDL_GetTicks();
    static u64 last_remainder = 1000;
    u64 current_remainder = current_ms % 1000;
    u64 temp = last_remainder;
    last_remainder = current_remainder;
    if (current_remainder >= temp)
        return;

    char buf[32];

    if(hz < 1000){
        snprintf(buf, sizeof(buf), "EMU hz: %u", hz);
    } else if(hz < 1000000) {
        snprintf(buf, sizeof(buf), "EMU khz: %.4f", (f32)(hz) / 1000);
    } else {
        snprintf(buf, sizeof(buf), "EMU mhz: %.4f", (f32)(hz) / 1000000);
    }

    SDLite_Text_SetString(hz_text, buf);
}