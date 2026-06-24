#include "pipeline/decode.h"


int decode(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u64 raw_instruction = instruction->raw_instruction;

    switch(instruction->addressing_mode){ // operand 1 is usually regC 

        case ADDR_REG:
            instruction->operands[0] = (raw_instruction >> 8) & 0x0f; 
            instruction->operands[2] = (raw_instruction >> 12) & 0x0f; 
            
            instruction->operands[1] = (raw_instruction >> 16) & 0x1F;
            break;

        case ADDR_REG_INDIRECT:
            instruction->operands[0] = (raw_instruction >> 8) & 0x01f;
            instruction->operands[1] = (raw_instruction >> 16) & 0x01f; 
            break;


        case ADDR_REG_IMMEDIATE_EIGHT:
            instruction->operands[0] = (raw_instruction >> 8) & 0x0f; 
            instruction->operands[1] = (raw_instruction >> 16) & 0x1f;
            instruction->operands[2] = (raw_instruction >> 24) & 0xff; 
            break;

        case ADDR_REG_IMMEDIATE_SIXTEEN:
            instruction->operands[0] = (raw_instruction >> 8) & 0x0f; 
            instruction->operands[1] = (raw_instruction >> 16) & 0x1f;
            instruction->operands[2] = (raw_instruction >> 24) & 0xffff; 
            break;

        case ADDR_IMMEDIATE_EIGHT_DEST:
            instruction->operands[0] = (raw_instruction >> 16) & 0xff; 
            instruction->operands[1] = (raw_instruction >> 8) & 0x1f;
            break;

        case ADDR_IMMEDIATE_SIXTEEN_DEST:
            instruction->operands[0] = (raw_instruction >> 16) & 0xffff; 
            instruction->operands[1] = (raw_instruction >> 8) & 0x1f;
            break;

        case ADDR_IMMEDIATE_EIGHT:
            instruction->operands[0] = (raw_instruction >> 8) & 0xff; 
            break;
            
        case ADDR_IMMEDIATE_SIXTEEN:
            instruction->operands[0] = (raw_instruction >> 8) & 0xffff; 
            break;
    }

    
    printf("operand 0: %d, operand 1: %d, operand 2: %d\n", instruction->operands[0], instruction->operands[1], instruction->operands[2]);


    return 1;
}