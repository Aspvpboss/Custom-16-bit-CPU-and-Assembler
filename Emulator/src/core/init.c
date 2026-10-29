#include "init.h"
#include "pipeline/instruction.h"

#define FONT_PATH "assets/font.ttf"
#define SDL_WHITE (SDL_Color){255, 255, 255, 255}

Emulator* init(){

    Emulator *emu = t_malloc(sizeof(Emulator));
    if(!emu){
        SDLite_SetError("init(): failed to allocate Emulator");
        return NULL;
    }

    emu->SDLite_io.input = SDLite_CreateInput();
    if(!emu->SDLite_io.input){
        SDLite_SetError("init(): failed to create SDLite_Input");
        t_free(emu);
        return NULL;
    }


    emu->SDLite_io.time = SDLite_CreateTime(1000);
    if(!emu->SDLite_io.time){
        SDLite_SetError("init(): failed to create SDLite_Time");
        SDLite_DestroyInput(emu->SDLite_io.input); 
        t_free(emu);
        return NULL;
    }

    EMU_init_alu(&emu->alu);
    EMU_init_fpu(&emu->fpu);

    memset(emu->ram.bootloader, 0, sizeof(emu->ram.bootloader));
    memset(emu->ram.ram, 0, sizeof(emu->ram.ram));
    memset(emu->ram.vram_zero, 0, sizeof(emu->ram.vram_zero));
    memset(emu->ram.vram_one, 0, sizeof(emu->ram.vram_one));
    memset(emu->ram.mmio, 0, sizeof(emu->ram.mmio));
    memset(emu->ram.ram_bank, 0, sizeof(emu->ram.ram_bank));
    emu->ram.vram_selector = false;

    emu->ram.bootloader[0] = 0x6 << 5 | EMU_JMP; 
    emu->ram.bootloader[1] = 0x0000;

    return emu;
}


int create_render_data(RenderData *render_data) {
    render_data->scale = RENDER_SCALE;
    render_data->height = EMU_DISPLAY_SIZE * RENDER_SCALE + TOP_BAR_TEXT_HEIGHT; 
    render_data->width = EMU_DISPLAY_SIZE * RENDER_SCALE;

    render_data->display = SDLite_CreateDisplay("Rose EMU", render_data->width, render_data->height, SDL_WINDOW_MAXIMIZED);
    if(!render_data->display) return 1;
  
    /*
        SDL_PIXELFORMAT_INDEX8 for indexing into a pallete
        SDL_PIXEL_FORMAT_RGB332 for 8-bit color
    */
    SDL_Renderer *renderer = SDLite_Display_GetSDLRenderer(render_data->display);
    render_data->emu_texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 
        EMU_DISPLAY_SIZE * RENDER_SCALE, EMU_DISPLAY_SIZE * RENDER_SCALE);
    if(!render_data->emu_texture) return 1;
    
    const SDL_Color colors[] = {
        {0, 0, 0, 0,},
        {255, 0, 0, 255},
        {0, 255, 0, 255}
    };
    render_data->palette = SDL_CreatePalette(sizeof(colors) / sizeof(SDL_Color));
    if(!render_data->palette) return 1;
    render_data->render_rect.x = 0.0f;
    render_data->render_rect.y = TOP_BAR_TEXT_HEIGHT;
    render_data->render_rect.w = EMU_DISPLAY_SIZE * RENDER_SCALE;
    render_data->render_rect.h = EMU_DISPLAY_SIZE * RENDER_SCALE;

    SDL_SetPaletteColors(render_data->palette, colors, 0, sizeof(colors) / sizeof(SDL_Color));
    SDL_SetTexturePalette(render_data->emu_texture, render_data->palette);

    render_data->font = SDLite_Create_Text_Font(FONT_PATH, TOP_BAR_TEXT_HEIGHT);
    if(!render_data->font) return 1;
    render_data->emu_hz = SDLite_CreateText(render_data->display, NULL, render_data->font, 0, 0, SDL_WHITE);
    if(!render_data->emu_hz) return 1;

    return 0;
}