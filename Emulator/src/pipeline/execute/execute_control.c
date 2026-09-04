#include "pipeline/execute/execute_control.h"
#include "hardware/flags.h"

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
            u16 jmp_addr = emu->alu.registers[instruction->operands[OP_ONE]];
            emu->alu.registers[ALU_PROGRAM_COUNTER] = jmp_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:{
            u16 offset_addr = instruction->operands[OP_ONE];
            emu->alu.registers[ALU_PROGRAM_COUNTER] += offset_addr;
            break;
        }
        case ADDR_IMMEDIATE_EIGHT:
        case ADDR_IMMEDIATE_SIXTEEN:{
            u16 jmp_addr = instruction->operands[OP_ONE];
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
    enum CMP_FlagMasks flags; 

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:
            break;
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:
            break;

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

        case ADDR_REG_INDIRECT:
            break;
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:
            break;
        case ADDR_IMMEDIATE_EIGHT:
        case ADDR_IMMEDIATE_SIXTEEN:
            break;

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

    switch(instruction->addressing_mode){

        case ADDR_REG_INDIRECT:
            break;
        case ADDR_IMMEDIATE_EIGHT_DEST:
        case ADDR_IMMEDIATE_SIXTEEN_DEST:
            break;

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

        case ADDR_IMMEDIATE_EIGHT:
            break;

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