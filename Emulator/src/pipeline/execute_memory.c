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


    if(address > 0x6EFF && address < 0x7F00){ // VRAM

        if(sixteen_bit_write == true && address + 1 > 0x7EFF) return 1;

        u16 offset = address - 0x6F00; 

        active_vram[offset] = (value & 0x00ff);
        if(sixteen_bit_write) active_vram[offset + 1] = (value & 0xff00) >> 8;

    } else if(address > 0x7EFF && address < 0x8000){ // MMIO
     
        if(sixteen_bit_write == true && address + 1 > 0x7FFF) return 1;

        u16 offset = address - 0x7F00;

        ram->mmio[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->mmio[offset + 1] = (value & 0xff00) >> 8;

    } else{ // RAM

        if(sixteen_bit_write == true && address + 1 > 0x6EFF) return 1;

        u16 offset = address - 0x0100;

        ram->ram[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->ram[offset + 1] = (value & 0xff00) >> 8;   
        
    }


    return 0;
}


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read){

    if(!ram || !value) return 1;

    u8 *active_vram = ram->vram_selector ? ram->vram_two : ram->vram_one;


    if(address > 0x6EFF && address < 0x7F00){ // VRAM

        if(sixteen_bit_read == true && address + 1 > 0x7EFF) return 1;

        u16 offset = address - 0x6F00; 

        *value = active_vram[offset];
        *value |= sixteen_bit_read ? (active_vram[offset + 1] << 8) : 0;

    } else if(address > 0x7EFF && address < 0x8000){ // MMIO
     
        if(sixteen_bit_read == true && address + 1 > 0x7FFF) return 1;

        u16 offset = address - 0x7F00;

        *value = ram->mmio[offset];
        *value |= sixteen_bit_read ? (ram->mmio[offset + 1] << 8) : 0;
    
    } else if(address > 0x7FFF){ // ROM bank

        if(sixteen_bit_read == true && address + 1 > 0xFFFF) return 1;

        u16 offset = address - 0x8000;

        *value = ram->rom_bank[offset];
        *value |= sixteen_bit_read ? (ram->rom_bank[offset + 1] << 8) : 0;

    } else if(address < 0x100){ // bootloader

        if(sixteen_bit_read == true && address + 1 > 0x00FF) return 1;

        *value = ram->bootloader[address];
        *value |= sixteen_bit_read ? (ram->bootloader[address + 1] << 8) : 0;


    } else{ // RAM

        if(sixteen_bit_read == true && address + 1 > 0x6EFF) return 1;

        u16 offset = address - 0x0100;

        *value = ram->ram[offset];
        *value |= sixteen_bit_read ? (ram->ram[offset + 1] << 8) : 0;
        
    }


    return 0;
}



int memory_test(EMU_Ram *ram){

    u16 destination_value = 0;
   
    if(write_memory(ram, 0x0101, 0xffff, true)){
        printf("simple write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x0101, &destination_value, true)){
        printf("simple read_memory failed\n");
        return 1;
    }
    printf("\n%d\n", destination_value);

    if(write_memory(ram, 0x6F01, 0xffff, false)){
        printf("vram write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x6F01, &destination_value, true)){
        printf("vram read_memory failed\n");
        return 1;
    }
    if(destination_value != 0xff){
        printf("vram read_memory failed, expected 0xff, got %d\n", destination_value);
        return 1;
    }
    printf("%d\n", destination_value);


    if(write_memory(ram, 0x7F01, 0xffff, false)){
        printf("mmio write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x7F01, &destination_value, true)){
        printf("mmio read_memory failed\n");
        return 1;
    }
    if(destination_value != 0xff){
        printf("mmio read_memory failed, expected 0xff, got %d\n", destination_value);
        return 1;
    }
    printf("%d\n", destination_value);


    if(!write_memory(ram, 0xffff, 0x00ff, false)){
        printf("write_memory failed, tried to write to rom failed\n");
        return 1;
    }
    if(!write_memory(ram, 0x0000, 0x00ff, false)){
        printf("write_memory failed, tried to write to bootloader failed\n");
        return 1;
    }

    if(!read_memory(ram, 0xffff, &destination_value, true)){
        printf("read_memory failed, tried to read from outside memory\n");
        return 1;
    }
    if(read_memory(ram, 0x0000, &destination_value, false)){
        printf("read_memory failed, failed to read from bootloader\n");
        return 1;
    }

    if(!write_memory(ram, 0x6EFF, 0xffff, true)){
        printf("write_memory failed, tried to write outside of ram\n");
        return 1;
    }




    return 0;
}

