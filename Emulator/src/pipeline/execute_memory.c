#include "pipeline/execute_memory.h"

/*
    0x0000 - 0x00FF: hardcoded bootloader (256B)

    0x0100 - 0x6EFF: RAM (~27.5 KiB)

    0x6F00 - 0x7EFF: VRAM (4 KiB, 64x64 @ 1 byte/pixel)

    0x7F00 - 0x7FFF: MMIO (256B)

    0x8000 - 0xFFFF: Selected ROM bank (32 KiB for each bank)
        can support techinically up to 2 ^ 16 amount of banks,
        each bank being a binary file that is loaded
*/



int write_memory(EMU_Ram *ram, u16 address, u16 value, bool sixteen_bit_write){

    if(!ram) return 1;

    return 0;
}


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read){

    return 0;
}
