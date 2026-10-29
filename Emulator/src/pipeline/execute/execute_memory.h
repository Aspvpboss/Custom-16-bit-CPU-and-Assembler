#ifndef EXECUTE_MEMORY_EMU_H
#define EXECUTE_MEMORY_EMU_H


#include "hardware/emulator.h"
#include "pipeline/instruction.h"
#include "core/test.h"

enum Move_Opcodes{

    MOV_REG_TO_REG,
    MOV_REG_TO_FLOAT,
    MOV_FLOAT_TO_REG,
    MOV_FLOAT_TO_FLOAT,

};


int read_memory(Emulator *emu, u16 address, u16 *value, bool sixteen_bit_read);
int write_memory(Emulator *emu, u16 address, u16 value, bool sixteen_bit_write);

static inline int exe_push(Emulator *emu, EMU_Decoded_Instruction *instruction){
   
    if(instruction->addressing_mode != ADDR_IMMEDIATE_EIGHT){
        SDLite_SetError("exe_push was given an invalid addressing mode: %d", instruction->addressing_mode);
        return 1;
    }

    bool sixteen_bit_write = IS_SIXTEEN_BIT_REG(instruction->operands[0]);
    u8 reg_read_addr = STRIP_FIFTH_BIT(instruction->operands[0]);

    u16 memory_value = emu->alu.registers[reg_read_addr];
    if(write_memory(emu, emu->alu.registers[ALU_STACK_POINTER], memory_value, sixteen_bit_write)){
        SDLite_SetError("exe_push failed to write to memory at 0x%04X", emu->alu.registers[ALU_STACK_POINTER]);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }
   
    emu->alu.registers[ALU_STACK_POINTER] -= sixteen_bit_write ? 2 : 1;

    return 0;
}


static inline int exe_pop(Emulator *emu, EMU_Decoded_Instruction *instruction){

    if(instruction->addressing_mode != ADDR_IMMEDIATE_EIGHT){
        SDLite_SetError("exe_pop was given an invalid addressing mode: %d", instruction->addressing_mode);
        return 1;
    }
    
    bool sixteen_bit_read = IS_SIXTEEN_BIT_REG(instruction->operands[0]);
    u8 reg_write_addr = STRIP_FIFTH_BIT(instruction->operands[0]);

    u16 memory_value = 0;
    emu->alu.registers[ALU_STACK_POINTER] += sixteen_bit_read ? 2 : 1;
    if(read_memory(emu, emu->alu.registers[ALU_STACK_POINTER], &memory_value, sixteen_bit_read)){
        SDLite_SetError("exe_pop failed to read from memory at 0x%04X", emu->alu.registers[ALU_STACK_POINTER]);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    emu->alu.registers[reg_write_addr] = memory_value;

    return 0;
}

static inline int exe_str(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool sixteen_bit_write = IS_SIXTEEN_BIT_REG(instruction->operands[1]);
    u8 reg_read_addr = STRIP_FIFTH_BIT(instruction->operands[1]);
    u16 reg_value = emu->alu.registers[reg_read_addr];
   
    bool write_failed = true;

    switch(instruction->addressing_mode){
         case ADDR_REG_INDIRECT:{
            u16 write_addr = emu->alu.registers[instruction->operands[0]];
            write_failed = write_memory(emu, write_addr, reg_value, sixteen_bit_write);
            break;
        }
        case ADDR_REG_IMMEDIATE_EIGHT:
        case ADDR_REG_IMMEDIATE_SIXTEEN: {
            u16 write_addr = emu->alu.registers[instruction->operands[0]] + instruction->operands[2];
            write_failed = write_memory(emu, write_addr, reg_value, sixteen_bit_write);
            break;
        }

        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST: {
            u16 write_addr = instruction->operands[0];
            write_failed = write_memory(emu, write_addr, reg_value, sixteen_bit_write);
            break;
        }
        default:
            write_failed = true;
            SDLite_SetError("exe_str was given an invalid addressing mode: %d", instruction->addressing_mode);
            break;
       
    }
    
    if(write_failed){
        SDLite_SetError("exe_str failed to write to memory (raw: 0x%llX)", (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }
    
    return 0;
}

static inline int exe_load(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool sixteen_bit_read = IS_SIXTEEN_BIT_REG(instruction->operands[1]);
    u16 memory_read_value = 0;
   
    bool read_failed = false;

    switch(instruction->addressing_mode){
        case ADDR_REG_INDIRECT:{
            u16 read_addr = emu->alu.registers[instruction->operands[0]];
            read_failed = read_memory(emu, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }
        case ADDR_REG_IMMEDIATE_EIGHT:
        case ADDR_REG_IMMEDIATE_SIXTEEN: {
            u16 read_addr = emu->alu.registers[instruction->operands[0]] + instruction->operands[2];
            read_failed = read_memory(emu, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }

        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST: {
            u16 read_addr = instruction->operands[0];
            read_failed = read_memory(emu, read_addr, &memory_read_value, sixteen_bit_read);
            break;
        }
        default:
            read_failed = true;
            SDLite_SetError("exe_load was given an invalid addressing mode: %d", instruction->addressing_mode);
            break;
    }

    if(read_failed){
        SDLite_SetError("exe_load failed to read from memory (raw: 0x%llX)", (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    u8 reg_write_addr = STRIP_FIFTH_BIT(instruction->operands[1]);
    emu->alu.registers[reg_write_addr] = memory_read_value;
    
    return 0;
}

static inline int exe_mov(Emulator *emu, EMU_Decoded_Instruction *instruction){

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

    SDLite_SetError("exe_mov was given an invalid move mode: %d", instruction->operands[2]);
    return 1;
}


#endif