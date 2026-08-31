#include "pipeline/fetch.h"
#include "pipeline/execute/execute_memory.h"
#include "core/test.h"


int fetch(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Ram *ram = &emu->ram;
    u16 *program_counter = &emu->alu.registers[ALU_PROGRAM_COUNTER];

    if((*program_counter) == __UINT16_MAX__ / 2){
        (*program_counter) = 0;
    } 
    
    u8 extra_bytes = 0;

    if(read_memory(ram, (*program_counter), (u16*)&instruction->raw_instruction, false)) return 1;

    instruction->addressing_mode = (EMU_Addressing_Modes)(instruction->raw_instruction >> 5);

    // flag addressing modes
    switch(instruction->addressing_mode){

        case ADDR_REG:
            extra_bytes = 2;
            break;

        case ADDR_REG_INDIRECT:
            extra_bytes = 2;
            break;
            
        case ADDR_REG_IMMEDIATE_EIGHT:
            extra_bytes = 3;
            break;

        case ADDR_REG_IMMEDIATE_SIXTEEN:
            extra_bytes = 4;
            break;

        case ADDR_IMMEDIATE_EIGHT_DEST:
            extra_bytes = 2;
            break;

        case ADDR_IMMEDIATE_SIXTEEN_DEST:
            extra_bytes = 3;
            break;

        case ADDR_IMMEDIATE_EIGHT:
            extra_bytes = 1;
            break;

        case ADDR_IMMEDIATE_SIXTEEN:
            extra_bytes = 2;
            break;

        default:
            return -1;
    }


    for(u8 i = 1; i <= extra_bytes; i++){
        (*program_counter)++;
        u8 raw_byte;
        if(read_memory(ram, (*program_counter), (u16*)&raw_byte, false)) return 1;
        instruction->raw_instruction |= raw_byte << (8 * i);
        if((*program_counter) == __UINT16_MAX__){
            (*program_counter) = 0;
        } 
    }

    (*program_counter)++;

    return 0;
}
