#ifndef EMULATOR_H
#define EMULATOR_H

#include <SDLite/SDLite.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_mutex.h>

#include "my_stdtypes.h"
#include "hardware/flags.h"
#include "hardware/alu.h"
#include "hardware/fpu.h"
#include "shared_data.h"


typedef struct{

    SDLite_Time *time;
    SDLite_Input *input;

} SDLite_IO;

typedef struct{

    u8 bootloader[0x100];
    u8 ram[0x6E00];

    u8 vram_zero[VRAM_SIZE];
    u8 vram_one[VRAM_SIZE];
    u8 mmio[0x100];
    u8 ram_bank[0x8000];
    bool vram_selector; // false vram_zero, true vram_one

} EMU_Ram;


typedef struct{

    EMU_Ram ram;
    SDLite_IO SDLite_io;
    EMU_Alu alu;
    EMU_Fpu fpu;
    CMP_Flags flags;
    SharedData *shared_data; // set after emulator creation

} Emulator;

#define IS_SIXTEEN_BIT_REG(reg) (reg & 0x0010) >> 4
#define STRIP_FIFTH_BIT(reg) (reg & 0b1111)

int emulator_main(void *ptr);


#endif