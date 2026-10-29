#include "pipeline/execute/execute_memory.h"



int write_memory(EMU_Ram *ram, u16 address, u16 value, bool sixteen_bit_write){

    if(!ram){
        SDLite_SetError("write_memory(): ram pointer is NULL");
        return 1;
    }

    if(address < 0x100){
        SDLite_SetError("write_memory(): attempted write to read-only bootloader at 0x%04X", address);
        return 1;
    }

    u8 *active_vram = ram->vram_selector ? ram->vram_one : ram->vram_zero;


    if(address >= 0x0100 && address < 0x6F00){ // RAM

        if(sixteen_bit_write == true && address + 1 >= 0x6F00){
            SDLite_SetError("write_memory(): 16-bit write out of RAM bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x0100;

        ram->ram[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->ram[offset + 1] = (value & 0xff00) >> 8;


    } else if(address >= 0x6F00 && address < 0x7F00){ // VRAM

        if(sixteen_bit_write == true && address + 1 > 0x7EFF){
            SDLite_SetError("write_memory(): 16-bit write out of VRAM bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x6F00; 


        active_vram[offset] = (value & 0x00ff);
        if(sixteen_bit_write) active_vram[offset + 1] = (value & 0xff00) >> 8;

    } 
    
    else if(address >= 0x7F00 && address < 0x8000){ // MMIO
     
        if(sixteen_bit_write == true && address + 1 > 0x7FFF){
            SDLite_SetError("write_memory(): 16-bit write out of MMIO bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x7F00;

        ram->mmio[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->mmio[offset + 1] = (value & 0xff00) >> 8;

    } else if (address >= 0x8000){

        if(sixteen_bit_write == true && (u32)(address + 1) > 0xFFFF){
            SDLite_SetError("write_memory(): 16-bit write out of RAM bank bounds at 0x%04X", address);
            return 1;
        }
        
        u16 offset = address - 0x8000;

        ram->ram_bank[offset] = value & 0x00ff;
        if(sixteen_bit_write) ram->ram_bank[offset + 1] = (value & 0xff00) >> 8;
        
    }


    return 0;
}


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read){

    if(!ram || !value){
        SDLite_SetError("read_memory(): NULL pointer argument");
        return 1;
    }

    u8 *active_vram = ram->vram_selector ? ram->vram_one : ram->vram_zero;

    if(address < 0x100){ // bootloader

        if(sixteen_bit_read == true && address + 1 > 0x00FF){
            SDLite_SetError("read_memory(): 16-bit read out of bootloader bounds at 0x%04X", address);
            return 1;
        }

        *value = ram->bootloader[address];
        *value |= sixteen_bit_read ? (ram->bootloader[address + 1] << 8) : 0;


    } else if(address >= 0x0100 && address < 0x6F00){ // RAM

        if(sixteen_bit_read == true && address + 1 > 0x6EFF){
            SDLite_SetError("read_memory(): 16-bit read out of RAM bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x0100;

        *value = ram->ram[offset];
        *value |= sixteen_bit_read ? (ram->ram[offset + 1] << 8) : 0;
        
    } else if(address >= 0x6F00 && address < 0x7F00){ // VRAM

        if(sixteen_bit_read == true && address + 1 > 0x7EFF){
            SDLite_SetError("read_memory(): 16-bit read out of VRAM bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x6F00; 
        

        *value = active_vram[offset];
        *value |= sixteen_bit_read ? (active_vram[offset + 1] << 8) : 0;
        

    } else if(address >= 0x7F00 && address < 0x8000){ // MMIO
     
        if(sixteen_bit_read == true && address + 1 > 0x7FFF){
            SDLite_SetError("read_memory(): 16-bit read out of MMIO bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x7F00;

        *value = ram->mmio[offset];
        *value |= sixteen_bit_read ? (ram->mmio[offset + 1] << 8) : 0;
    
    } else if(address >= 0x8000){ // RAM bank

        if(sixteen_bit_read == true && address + 1 > 0xFFFF){
            SDLite_SetError("read_memory(): 16-bit read out of RAM bank bounds at 0x%04X", address);
            return 1;
        }

        u16 offset = address - 0x8000;

        *value = ram->ram_bank[offset];
        *value |= sixteen_bit_read ? (ram->ram_bank[offset + 1] << 8) : 0;

    }

    return 0;
}
