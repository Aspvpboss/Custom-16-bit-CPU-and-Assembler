#include "init.h"



Emulator* init(){


    Emulator *emu = t_malloc(sizeof(Emulator));
    if(!emu) return NULL;


    emu->SDLite_io.input = SDLite_CreateInput();
    if(!emu->SDLite_io.input){
        t_free(emu);
        return NULL;
    }

    emu->SDLite_io.display = SDLite_CreateDisplay("EMU", 800, 800, SDL_WINDOW_MAXIMIZED);
    if(!emu->SDLite_io.display){
        SDLite_DestroyInput(emu->SDLite_io.input); 
        t_free(emu);
        return NULL;
    }

    emu->SDLite_io.time = SDLite_CreateTime(1000);
    if(!emu->SDLite_io.time){
        SDLite_DestroyInput(emu->SDLite_io.input); 
        SDLite_DestroyDisplay(emu->SDLite_io.display);
        t_free(emu);
        return NULL;
    }

    EMU_init_alu(&emu->alu);
    EMU_init_fpu(&emu->fpu);

    memset(emu->ram.bootloader, 0, sizeof(emu->ram.bootloader));
    memset(emu->ram.ram, 0, sizeof(emu->ram.ram));
    memset(emu->ram.vram_one, 0, sizeof(emu->ram.vram_one));
    memset(emu->ram.vram_two, 0, sizeof(emu->ram.vram_two));
    memset(emu->ram.mmio, 0, sizeof(emu->ram.mmio));
    memset(emu->ram.ram_bank, 0, sizeof(emu->ram.ram_bank));
    emu->ram.vram_selector = false;

    emu->program_counter = 0;


    return emu;
}