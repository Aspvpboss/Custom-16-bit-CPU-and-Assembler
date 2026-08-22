#include "pipeline/execute_memory.h"

/*
    0x0000 - 0x00FF: hardcoded bootloader (256B)

    0x0100 - 0x6EFF: RAM (~27.5 KiB)

    0x6F00 - 0x7EFF: VRAM (4 KiB, 64x64 @ 1 byte/pixel)

    0x7F00 - 0x7FFF: MMIO (256B)

    0x8000 - 0xFFFF: Selected RAM bank (32 KiB for each bank)
        can support techinically up to 2 ^ 16 amount of banks,
        each bank being a binary file that is loaded
*/



int write_memory(EMU_Ram *ram, u16 address, u16 value, bool sixteen_bit_write){

    if(!ram) return 1;

    if(address < 0x100) return 1;

    u8 *active_vram = ram->vram_selector ? ram->vram_two : ram->vram_one;


    if(address >= 0x0100 && address < 0x6F00){ // RAM

        if(sixteen_bit_write == true && address + 1 >= 0x6F00) return 1;

        u16 offset = address - 0x0100;

        ram->ram[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->ram[offset + 1] = (value & 0xff00) >> 8;


    } else if(address >= 0x6F00 && address < 0x7F00){ // VRAM

        if(sixteen_bit_write == true && address + 1 > 0x7EFF) return 1;

        u16 offset = address - 0x6F00; 

        active_vram[offset] = (value & 0x00ff);
        if(sixteen_bit_write) active_vram[offset + 1] = (value & 0xff00) >> 8;

    } 
    
    else if(address >= 0x7F00 && address < 0x8000){ // MMIO
     
        if(sixteen_bit_write == true && address + 1 > 0x7FFF) return 1;

        u16 offset = address - 0x7F00;

        ram->mmio[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->mmio[offset + 1] = (value & 0xff00) >> 8;

    } else if (address >= 0x8000){

        if(sixteen_bit_write == true && (u32)(address + 1) > 0xFFFF) return 1;
        
        u16 offset = address - 0x8000;

        ram->ram_bank[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->ram_bank[offset + 1] = (value & 0xff00) >> 8;
        
    }


    return 0;
}


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read){

    if(!ram || !value) return 1;

    u8 *active_vram = ram->vram_selector ? ram->vram_two : ram->vram_one;

    if(address < 0x100){ // bootloader

        if(sixteen_bit_read == true && address + 1 > 0x00FF) return 1;

        *value = ram->bootloader[address];
        *value |= sixteen_bit_read ? (ram->bootloader[address + 1] << 8) : 0;


    } else if(address >= 0x0100 && address < 0x6F00){ // RAM

        if(sixteen_bit_read == true && address + 1 > 0x6EFF) return 1;

        u16 offset = address - 0x0100;

        *value = ram->ram[offset];
        *value |= sixteen_bit_read ? (ram->ram[offset + 1] << 8) : 0;
        
    } else if(address >= 0x6F00 && address < 0x7F00){ // VRAM

        if(sixteen_bit_read == true && address + 1 > 0x7EFF) return 1;

        u16 offset = address - 0x6F00; 

        *value = active_vram[offset];
        *value |= sixteen_bit_read ? (active_vram[offset + 1] << 8) : 0;

    } else if(address >= 0x7F00 && address < 0x8000){ // MMIO
     
        if(sixteen_bit_read == true && address + 1 > 0x7FFF) return 1;

        u16 offset = address - 0x7F00;

        *value = ram->mmio[offset];
        *value |= sixteen_bit_read ? (ram->mmio[offset + 1] << 8) : 0;
    
    } else if(address >= 0x8000){ // RAM bank

        if(sixteen_bit_read == true && address + 1 > 0xFFFF) return 1;

        u16 offset = address - 0x8000;

        *value = ram->ram_bank[offset];
        *value |= sixteen_bit_read ? (ram->ram_bank[offset + 1] << 8) : 0;

    }

    return 0;
}


