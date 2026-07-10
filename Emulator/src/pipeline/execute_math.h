#ifndef EXECUTE_MATH_EMU_H
#define EXECUTE_MATH_EMU_H

#include "emulator/emulator.h"
#include "pipeline/instruction.h"


int exe_add(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_sub(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_mul(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_div(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_mod(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_cmp(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_and(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_nor(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_xor(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_ars(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_lrs(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_lls(Emulator *emu, EMU_Decoded_Instruction *instruction);

int exe_fadd(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fsub(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fmul(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fdiv(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fsqrt(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fcmp(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_fint(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_iflo(Emulator *emu, EMU_Decoded_Instruction *instruction);


#endif