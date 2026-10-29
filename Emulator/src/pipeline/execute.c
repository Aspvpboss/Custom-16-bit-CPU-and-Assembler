
#include "pipeline/execute.h"
#include "execute/execute_math.h"
#include "execute/execute_memory.h"
#include "execute/execute_control.h"

int execute(Emulator *emu, EMU_Decoded_Instruction *instruction){

    static const void * const dispatch_table[] = {
        [EMU_LOAD]    = &&L_EMU_LOAD,
        [EMU_STR]     = &&L_EMU_STR,
        [EMU_PUSH]    = &&L_EMU_PUSH,
        [EMU_POP]     = &&L_EMU_POP,
        [EMU_MOV]     = &&L_EMU_MOV,
        [EMU_SYSCALL] = &&L_EMU_SYSCALL,
        [EMU_JMP]     = &&L_EMU_JMP,
        [EMU_JIF]     = &&L_EMU_JIF,
        [EMU_CAL]     = &&L_EMU_CAL,
        [EMU_CIF]     = &&L_EMU_CIF,
        [EMU_RET]     = &&L_EMU_RET,
        [EMU_ADD]     = &&L_EMU_ADD,
        [EMU_SUB]     = &&L_EMU_SUB,
        [EMU_MUL]     = &&L_EMU_MUL,
        [EMU_DIV]     = &&L_EMU_DIV,
        [EMU_MOD]     = &&L_EMU_MOD,
        [EMU_CMP]     = &&L_EMU_CMP,
        [EMU_AND]     = &&L_EMU_AND,
        [EMU_NOR]     = &&L_EMU_NOR,
        [EMU_XOR]     = &&L_EMU_XOR,
        [EMU_ARS]     = &&L_EMU_ARS,
        [EMU_LRS]     = &&L_EMU_LRS,
        [EMU_LLS]     = &&L_EMU_LLS,
        [EMU_FADD]    = &&L_EMU_FADD,
        [EMU_FSUB]    = &&L_EMU_FSUB,
        [EMU_FMUL]    = &&L_EMU_FMUL,
        [EMU_FDIV]    = &&L_EMU_FDIV,
        [EMU_FSQRT]   = &&L_EMU_FSQRT,
        [EMU_FCMP]    = &&L_EMU_FCMP,
        [EMU_FINT]    = &&L_EMU_FINT,
        [EMU_IFLO]    = &&L_EMU_IFLO,
    };

    EMU_Instructions opcode = instruction->opcode;

    if(__builtin_expect((unsigned int)opcode > EMU_IFLO, 0)){
        SDLite_SetError("execute() was given an invalid opcode: %d", opcode);
        return 1;
    }

    goto *dispatch_table[opcode];

    L_EMU_LOAD:
        return exe_load(emu, instruction);

    L_EMU_STR:
        return exe_str(emu, instruction);

    L_EMU_PUSH:
        return exe_push(emu, instruction);

    L_EMU_POP:
        return exe_pop(emu, instruction);

    L_EMU_MOV:
        return exe_mov(emu, instruction);

    L_EMU_SYSCALL:
        return exe_syscall(emu, instruction);

    L_EMU_JMP:
        return exe_jmp(emu, instruction);

    L_EMU_JIF:
        return exe_jif(emu, instruction);

    L_EMU_CAL:
        return exe_cal(emu, instruction);

    L_EMU_CIF:
        return exe_cif(emu, instruction);

    L_EMU_RET:
        return exe_ret(emu, instruction);

    L_EMU_ADD:
        return exe_add(emu, instruction);

    L_EMU_SUB:
        return exe_sub(emu, instruction);

    L_EMU_MUL:
        return exe_mul(emu, instruction);

    L_EMU_DIV:
        return exe_div(emu, instruction);

    L_EMU_MOD:
        return exe_mod(emu, instruction);

    L_EMU_CMP:
        return exe_cmp(emu, instruction);

    L_EMU_AND:
        return exe_and(emu, instruction);

    L_EMU_NOR:
        return exe_nor(emu, instruction);

    L_EMU_XOR:
        return exe_xor(emu, instruction);

    L_EMU_ARS:
        return exe_ars(emu, instruction);

    L_EMU_LRS:
        return exe_lrs(emu, instruction);

    L_EMU_LLS:
        return exe_lls(emu, instruction);

    L_EMU_FADD:
        return exe_fadd(emu, instruction);

    L_EMU_FSUB:
        return exe_fsub(emu, instruction);

    L_EMU_FMUL:
        return exe_fmul(emu, instruction);

    L_EMU_FDIV:
        return exe_fdiv(emu, instruction);

    L_EMU_FSQRT:
        return exe_fsqrt(emu, instruction);

    L_EMU_FCMP:
        return exe_fcmp(emu, instruction);

    L_EMU_FINT:
        return exe_fint(emu, instruction);

    L_EMU_IFLO:
        return exe_iflo(emu, instruction);
}