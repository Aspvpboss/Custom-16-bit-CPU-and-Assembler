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

    if(address < 0x100 || address > 0x7FFF) return 1;

    u8 *active_vram = ram->vram_selector ? ram->vram_two : ram->vram_one;


    if(address > 0x6EFF && address < 0x7F00){

        if(sixteen_bit_write == true && address + 1 > 0x7EFF) return 1;

        u16 offset = address - 0x6F00; 

        active_vram[offset] = (value & 0x00ff);
        active_vram[offset + 1] = sixteen_bit_write ? (value & 0xff00) >> 8 : active_vram[offset + 1];

    } else if(address > 0x7EFF && address < 0x8000){
     
        if(sixteen_bit_write == true && address + 1 > 0x7FFF) return 1;

        u16 offset = address - 0x7F00;

        ram->mmio[offset] = value & 0x00ff;
        ram->mmio[offset + 1] = sixteen_bit_write ? (value & 0xff00) >> 8 : ram->mmio[offset + 1];

    } else{

        if(sixteen_bit_write == true && address + 1 > 0x6EFF) return 1;

        u16 offset = address - 0x0100;

        ram->ram[offset] = value & 0x00ff;
        ram->ram[offset + 1] = sixteen_bit_write ? (value & 0xff00) >> 8 : ram->ram[offset + 1];
        
    }


    return 0;
}


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read){

    if(!ram || !value) return 1;

    u8 *active_vram = ram->vram_selector ? ram->vram_two : ram->vram_one;


    if(sixteen_bit_read == true){



    } else if(sixteen_bit_read == false){



    }


    return 0;
}
