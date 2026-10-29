#ifndef EMU_UPDATE_H
#define EMU_UPDATE_H


#include "shared_data.h"
#include "hardware/emulator.h"

#define TOP_BAR_TEXT_HEIGHT 20
#define RENDER_SCALE 8

typedef struct {

    SDL_FRect render_rect;
    SDL_Texture *emu_texture;
    SDL_Palette *palette;
    SDLite_Display *display;
    SDLite_Text *emu_hz;
    SDLite_Text_Font *font;
    u32 width, height;
    u8 scale;

} RenderData;


void update_render_texture(SharedData *shared_data, RenderData *render);
void update_emu_hz_text(SDLite_Text *hz_text, u32 hz);

#endif