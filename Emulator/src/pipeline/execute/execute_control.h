#ifndef EXECUTE_CONTROL_EMU_H
#define EXECUTE_CONTROL_EMU_H

#include "hardware/emulator.h"
#include "pipeline/instruction.h"
#include "pipeline/execute/execute_memory.h"
#include "core/test.h"

int exe_syscall(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_jmp(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_jif(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_cal(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_cif(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_ret(Emulator *emu, EMU_Decoded_Instruction *instruction);

#endif