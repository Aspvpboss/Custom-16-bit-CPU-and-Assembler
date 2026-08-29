#ifndef EXECUTE_MEMORY_EMU_H
#define EXECUTE_MEMORY_EMU_H


#include "hardware/emulator.h"
#include "pipeline/instruction.h"
#include "core/test.h"

int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read);
int write_memory(EMU_Ram *ram, u16 address, u16 value, bool sixteen_bit_write);
int exe_push(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_pop(Emulator *emu, EMU_Decoded_Instruction *instruction);


#endif