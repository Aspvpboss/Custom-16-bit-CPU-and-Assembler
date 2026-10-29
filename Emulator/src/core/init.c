#include "init.h"
#include "pipeline/instruction.h"


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