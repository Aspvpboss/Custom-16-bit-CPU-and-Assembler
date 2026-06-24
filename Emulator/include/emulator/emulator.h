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

    u16 program_counter;
    u8 ram[0x7fff];
    SDLite_IO SDLite_io;
    EMU_Alu alu;
    EMU_Fpu fpu;
    CMP_Flags flags;

} Emulator;



int emulator_main(void *ptr);


#endif