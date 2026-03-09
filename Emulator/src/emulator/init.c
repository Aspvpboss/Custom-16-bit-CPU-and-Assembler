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
    memset(emu->ram, 0, sizeof(emu->ram));
    emu->program_counter = 0;

    emu->ram[0] = 0b10100000;
    emu->ram[1] = 0b01101010;
    emu->ram[2] = 0xff;
    emu->ram[3] = 0xff;
    emu->ram[4] = 0xaa;
    emu->ram[5] = 0xff;

    return emu;
}