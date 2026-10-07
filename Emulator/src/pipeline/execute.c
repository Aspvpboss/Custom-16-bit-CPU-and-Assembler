
#include "pipeline/execute.h"
#include "execute/execute_math.h"
#include "execute/execute_memory.h"


int execute(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Instructions opcode = EMU_JMP;
    // EMU_instruction opcode = instruction->opcode;


    switch(opcode){

        case EMU_LOAD:
            if(exe_load(emu, instruction)) return 1;
            break;

        case EMU_STR:
            if(exe_str(emu, instruction)) return 1;
            break;

        case EMU_PUSH:
            if(exe_push(emu, instruction)) return 1;
            break;

        case EMU_POP:
            if(exe_pop(emu, instruction)) return 1;
            break;

        case EMU_MOV:
            if(exe_mov(emu, instruction)) return 1;
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
            if(exe_add(emu, instruction)) return 1;
            break;

        case EMU_SUB:
            if(exe_sub(emu, instruction)) return 1;
            break;

        case EMU_MUL:
            if(exe_mul(emu, instruction)) return 1;
            break;

        case EMU_DIV:
            if(exe_div(emu, instruction)) return 1;
            break;

        case EMU_MOD:
            if(exe_mod(emu, instruction)) return 1;
            break;

        case EMU_CMP:
            if(exe_cmp(emu, instruction)) return 1;
            break;

        case EMU_AND:
            if(exe_and(emu, instruction)) return 1;
            break;

        case EMU_NOR:
            if(exe_nor(emu, instruction)) return 1;
            break;

        case EMU_XOR:
            if(exe_xor(emu, instruction)) return 1;
            break;

        case EMU_ARS:
            if(exe_ars(emu, instruction)) return 1;
            break;

        case EMU_LRS:
            if(exe_lrs(emu, instruction)) return 1;
            break;

        case EMU_LLS:
            if(exe_lls(emu, instruction)) return 1;
            break;

        case EMU_FADD:
            if(exe_fadd(emu, instruction)) return 1;
            break;

        case EMU_FSUB:
            if(exe_fsub(emu, instruction)) return 1;
            break;

        case EMU_FMUL:
            if(exe_fmul(emu, instruction)) return 1;
            break;

        case EMU_FDIV:
            if(exe_fdiv(emu, instruction)) return 1;
            break;

        case EMU_FSQRT:
            if(exe_fsqrt(emu, instruction)) return 1;
            break;

        case EMU_FCMP:
            if(exe_fcmp(emu, instruction)) return 1;
            break;

        case EMU_FINT:
            if(exe_fint(emu, instruction)) return 1;
            break;

        case EMU_IFLO:
            if(exe_iflo(emu, instruction)) return 1;
            break;

        default:
            break;
    }

    return 1;
}