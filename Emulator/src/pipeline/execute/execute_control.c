#include "pipeline/execute/execute_control.h"
#include "hardware/flags.h"


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
    emu->alu.registers[ALU_STACK_POINTER] -= 2;
    if(write_memory(&emu->ram, emu->alu.registers[ALU_STACK_POINTER], addr, true))
        return 1;

    return 0;
}

int exe_syscall(Emulator *emu, EMU_Decoded_Instruction *instruction){

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
        d_printf("exe_jmp failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

int exe_jmp(Emulator *emu, EMU_Decoded_Instruction *instruction){

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
        d_printf("exe_jmp failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

int exe_jif(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;
    enum CMP_FlagMasks check_flags = (enum CMP_FlagMasks)instruction->operands[OP_ONE]; 
    u16 current_flags = emu->flags;

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:{
            if(is_flag_on(current_flags, check_flags)) break;
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            if(is_flag_on(current_flags, check_flags)) break;
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        d_printf("exe_jif failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

int exe_cal(Emulator *emu, EMU_Decoded_Instruction *instruction){

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
        d_printf("exe_cal failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

int exe_cif(Emulator *emu, EMU_Decoded_Instruction *instruction){

    bool failure = false;
    enum CMP_FlagMasks check_flags = (enum CMP_FlagMasks)instruction->operands[OP_ONE]; 
    u16 current_flags = emu->flags;

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:{
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            if(is_flag_on(current_flags, check_flags)) break;
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ZERO]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            if(push_addr(emu, emu->alu.registers[ALU_PROGRAM_COUNTER])){
                failure = true;
                break;
            } 
            if(is_flag_on(current_flags, check_flags)) break;
            u16 offset_addr = instruction->operands[OP_ZERO];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }

        default:
            failure = true;
            break;
    }

    if(failure){
        d_printf("exe_cif failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}

int exe_ret(Emulator *emu, EMU_Decoded_Instruction *instruction){
    
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
        d_printf("exe_ret failed\n");
        PRINT_INDIVIDUAL_BYTES(instruction->raw_instruction);
        return 1;
    }

    return 0;
}