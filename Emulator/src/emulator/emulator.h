#ifndef EMULATOR_H
#define EMULATOR_H

#include "my_stdtypes.h"
#include <SDLite/SDLite.h>
#include <SDLite/SDL3/SDL_thread.h>
#include <SDLite/SDL3/SDL_atomic.h>

#include "emulator/flags.h"
#include "emulator/alu.h"
#include "emulator/fpu.h"


typedef struct{

    SDLite_Time *time;
    SDLite_Input *input;
    SDLite_Display *display;

} SDLite_IO;

typedef struct{

    u8 bootloader[0x100];
    u8 ram[0x6E00];

    bool vram_selector; // false vram_one, true vram_two
    u8 vram_one[0x1000];
    u8 vram_two[0x1000];
    u8 mmio[0x100];
    u8 ram_bank[0x8000];

} EMU_Ram;


typedef struct{

    u16 program_counter;
    EMU_Ram ram;
    SDLite_IO SDLite_io;
    EMU_Alu alu;
    EMU_Fpu fpu;
    CMP_Flags flags;

} Emulator;



int emulator_main(void *ptr);


#endif