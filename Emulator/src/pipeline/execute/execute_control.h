#ifndef EXECUTE_CONTROL_EMU_H
#define EXECUTE_CONTROL_EMU_H

#include "hardware/emulator.h"
#include "hardware/flags.h"
#include "pipeline/instruction.h"
#include "pipeline/execute/execute_memory.h"
#include "core/test.h"

static inline int is_flag_on(u16 alu_flags, CMP_Flags flag_to_check) {
    return (alu_flags >> flag_to_check) & 1;
}

static inline int pop_addr(Emulator *emu, u16 *addr) {
    emu->alu.registers[ALU_STACK_POINTER] += 2;
    
    if(read_memory(&emu->ram, emu->alu.registers[ALU_STACK_POINTER], addr, true)) 
        return 1;
    
    return 0;
}

static inline int push_addr(Emulator *emu, u16 addr) {
    if(write_memory(&emu->ram, emu->alu.registers[ALU_STACK_POINTER], addr, true))
        return 1;
    emu->alu.registers[ALU_STACK_POINTER] -= 2;

    return 0;
}

static inline int exe_syscall(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;

    switch(instruction->addressing_mode){
        case ADDR_IMMEDIATE_EIGHT:
        case ADDR_IMMEDIATE_SIXTEEN:
            // nothing here for now
            break;

        default:
            failure = true;
            break;
    }
    
    if(failure){
        SDLite_SetError("exe_syscall failed: invalid addressing mode %d (raw: 0x%llX)",
                        instruction->addressing_mode, (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

static inline int exe_jmp(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;

    switch(instruction->addressing_mode){
        
        case ADDR_REG_INDIRECT:{
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT:
        case ADDR_IMMEDIATE_SIXTEEN:{
            u16 jmp_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        SDLite_SetError("exe_jmp failed: invalid addressing mode %d (raw: 0x%llX)",
                        instruction->addressing_mode, (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

static inline int exe_jif(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;
    enum CMP_FlagMasks check_flags = (enum CMP_FlagMasks)instruction->operands[OP_ONE]; 
    u16 current_flags = emu->flags;

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:{
            if(!is_flag_on(current_flags, check_flags)) break;
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            if(!is_flag_on(current_flags, check_flags)) break;
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        SDLite_SetError("exe_jif failed: invalid addressing mode %d (raw: 0x%llX)",
                        instruction->addressing_mode, (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

static inline int exe_cal(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:{
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }
        case ADDR_IMMEDIATE_SIXTEEN:{
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            u16 jmp_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        SDLite_SetError("exe_cal failed (addressing mode: %d, SP: 0x%04X, raw: 0x%llX)",
                        instruction->addressing_mode, emu->alu.registers[ALU_STACK_POINTER],
                        (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

static inline int exe_cif(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;
    enum CMP_FlagMasks check_flags = (enum CMP_FlagMasks)instruction->operands[OP_ONE]; 
    u16 current_flags = emu->flags;

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:{
            if(!is_flag_on(current_flags, check_flags)) break;
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            if(!is_flag_on(current_flags, check_flags)) break;
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        SDLite_SetError("exe_cif failed (addressing mode: %d, SP: 0x%04X, raw: 0x%llX)",
                        instruction->addressing_mode, emu->alu.registers[ALU_STACK_POINTER],
                        (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

static inline int exe_ret(Emulator *emu, EMU_Decoded_Instruction *instruction){
    
    bool failure = false;

    switch(instruction->addressing_mode){

        case ADDR_IMMEDIATE_EIGHT:{
            u16 addr = 0;
            if(pop_addr(emu, &addr)){
                failure = true;
                break;
            }
            emu->alu.registers[ALU_PROGRAM_COUNTER] = addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        SDLite_SetError("exe_ret failed (addressing mode: %d, SP: 0x%04X, raw: 0x%llX)",
                        instruction->addressing_mode, emu->alu.registers[ALU_STACK_POINTER],
                        (unsigned long long)instruction->raw_instruction);
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

#endif