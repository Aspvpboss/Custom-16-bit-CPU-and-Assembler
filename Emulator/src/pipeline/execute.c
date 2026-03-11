#include "pipeline/execute.h"
#include "debug.h"


int execute(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Instructions opcode = EMU_JMP;
    // EMU_Instructions opcode = instruction->opcode;
    // EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    switch(opcode){

        case EMU_LOAD:
            break;

        case EMU_STR:
            break;

        case EMU_PUSH:
            break;

        case EMU_POP:
            break;

        case EMU_LDI:
            break;

        case EMU_MOV:
            break;

        case EMU_SYSCALL:
            break;

        case EMU_JMP:
            break;

        case EMU_JIF:
            break;

        case EMU_CAL:
            break;

        case EMU_CIF:
            break;

        case EMU_RET:
            break;

        case EMU_ADD:
            break;

        case EMU_SUB:
            break;

        case EMU_MUL:
            break;

        case EMU_DIV:
            break;

        case EMU_MOD:
            break;

        case EMU_CMP:
            break;

        case EMU_AND:
            break;

        case EMU_NOR:
            break;

        case EMU_XOR:
            break;

        case EMU_ARS:
            break;

        case EMU_LRS:
            break;

        case EMU_LLS:
            break;

        case EMU_FADD:
            break;

        case EMU_FSUB:
            break;

        case EMU_FMUL:
            break;

        case EMU_FDIV:
            break;

        case EMU_FSQRT:
            break;

        case EMU_FCMP:
            break;

        case EMU_FINT:
            break;

        case EMU_IFLO:
            break;

        default:
            break;
    }

    return 0;
}