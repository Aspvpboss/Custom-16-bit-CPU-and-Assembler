#include "pipeline/fetch.h"
#include "pipeline/execute/execute_memory.h"
#include "core/test.h"


int fetch(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Ram *ram = &emu->ram;

    if(emu->program_counter == __UINT16_MAX__ / 2){
        emu->program_counter = 0;
    } 
    
    u8 extra_bytes = 0;

    if(read_memory(ram, emu->program_counter, (u16*)&instruction->raw_instruction, false)) return 1;

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
        emu->program_counter++;
        if(read_memory(ram, emu->program_counter, (u16*)&instruction->raw_instruction, false)) return 1;
        if(emu->program_counter == __UINT16_MAX__ / 2){
            emu->program_counter = 0;
        } 
    }


    emu->program_counter++;
    emu->program_counter = 0;


    return 0;
}
