#include "pipeline/execute/execute_memory.h"



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




int exe_push(Emulator *emu, EMU_Decoded_Instruction *instruction){
   
    if(instruction->addressing_mode != ADDR_IMMEDIATE_EIGHT){
        d_printf("exe_push was given an invalid addressing mode\n");
        return 1;
    }

    bool sixteen_bit_write = IS_SIXTEEN_BIT_REG(instruction->operands[0]);
    u8 reg_read_addr = STRIP_FIFTH_BIT(instruction->operands[0]);

    u16 memory_value = emu->alu.registers[reg_read_addr];
    if(write_memory(&emu->ram, emu->alu.registers[ALU_STACK_POINTER], memory_value, sixteen_bit_write)){
        d_printf("exe_push failed to read from memory\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }
   
    emu->alu.registers[ALU_STACK_POINTER] -= sixteen_bit_write ? 2 : 1;

    return 0;
}


int exe_pop(Emulator *emu, EMU_Decoded_Instruction *instruction){

    if(instruction->addressing_mode != ADDR_IMMEDIATE_EIGHT){
        d_printf("exe_pop was given an invalid addressing mode\n");
        return 1;
    }
    
    bool sixteen_bit_read = IS_SIXTEEN_BIT_REG(instruction->operands[0]);
    u8 reg_write_addr = STRIP_FIFTH_BIT(instruction->operands[0]);

    u16 memory_value = 0;
    emu->alu.registers[ALU_STACK_POINTER] += sixteen_bit_read ? 2 : 1;
    if(read_memory(&emu->ram, emu->alu.registers[ALU_STACK_POINTER], &memory_value, sixteen_bit_read)){
        d_printf("exe_pop failed to read from memory\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    emu->alu.registers[reg_write_addr] = memory_value;

    return 0;
}

int exe_str(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool sixteen_bit_write = IS_SIXTEEN_BIT_REG(instruction->operands[1]);
    u8 reg_read_addr = STRIP_FIFTH_BIT(instruction->operands[1]);
    u16 reg_value = emu->alu.registers[reg_read_addr];
   
    bool write_failed = true;

    switch(instruction->addressing_mode){
         case ADDR_REG_INDIRECT:{
            u16 write_addr = emu->alu.registers[instruction->operands[0]];
            write_failed = write_memory(&emu->ram, write_addr, reg_value, sixteen_bit_write);
            break;
        }
        case ADDR_REG_IMMEDIATE_EIGHT:
        case ADDR_REG_IMMEDIATE_SIXTEEN: {
            u16 write_addr = emu->alu.registers[instruction->operands[0]] + instruction->operands[2];
            write_failed = write_memory(&emu->ram, write_addr, reg_value, sixteen_bit_write);
            break;
        }

        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST: {
            u16 write_addr = instruction->operands[0];
            write_failed = write_memory(&emu->ram, write_addr, reg_value, sixteen_bit_write);
            break;
        }
        default:
            write_failed = true;
            d_printf("exe_str was given an invalid addressing mode\n");
            break;
       
    }
    
    if(write_failed){
        d_printf("exe_str failed to read from memory\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }
    
    return 0;
}

int exe_load(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool sixteen_bit_read = IS_SIXTEEN_BIT_REG(instruction->operands[1]);
    u16 memory_read_value = 0;
   
    bool read_failed = false;

    switch(instruction->addressing_mode){
        case ADDR_REG_INDIRECT:{
            u16 read_addr = emu->alu.registers[instruction->operands[0]];
            read_failed = read_memory(&emu->ram, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }
        case ADDR_REG_IMMEDIATE_EIGHT:
        case ADDR_REG_IMMEDIATE_SIXTEEN: {
            u16 read_addr = emu->alu.registers[instruction->operands[0]] + instruction->operands[2];
            read_failed = read_memory(&emu->ram, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }

        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST: {
            u16 read_addr = instruction->operands[0];
            read_failed = read_memory(&emu->ram, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }
        default:
            read_failed = true;
            d_printf("exe_load was given an invalid addressing mode\n");
            break;
    }

    if(read_failed){
        d_printf("exe_load failed to read from memory\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    u8 reg_write_addr = STRIP_FIFTH_BIT(instruction->operands[1]);
    emu->alu.registers[reg_write_addr] = memory_read_value;
    
    return 0;
}

int exe_mov(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u8 src_addr = STRIP_FIFTH_BIT(instruction->operands[0]);
    u8 dest_addr = STRIP_FIFTH_BIT(instruction->operands[1]);

    u16 *int_registers = emu->alu.registers;
    f16 *float_registers = emu->fpu.registers;

    switch(instruction->operands[2]){
       
        case MOV_REG_TO_REG:{
            int_registers[dest_addr] = int_registers[src_addr];
            return 0;
        }
        case MOV_REG_TO_FLOAT:{
            float_registers[dest_addr] = int_registers[src_addr];
            return 0;
        }
        case MOV_FLOAT_TO_REG:{
            int_registers[dest_addr] = float_registers[src_addr];
            return 0;
        }
        case MOV_FLOAT_TO_FLOAT:{
            float_registers[dest_addr] = float_registers[src_addr];
            return 0;
        }
        
        default:
            break;
    } 

    d_printf("exe_mov was given an invalid addressing mode\n");
    return 1;
}