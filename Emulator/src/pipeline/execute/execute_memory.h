#ifndef EXECUTE_MEMORY_EMU_H
#define EXECUTE_MEMORY_EMU_H


#include "hardware/emulator.h"
#include "pipeline/instruction.h"
#include "core/test.h"

enum Move_Opcodes{

    MOV_REG_TO_REG,
    MOV_REG_TO_FLOAT,
    MOV_FLOAT_TO_REG,
    MOV_FLOAT_TO_FLOAT,

};


int read_memory(EMU_Ram *ram, u16 address, u16 *value, bool sixteen_bit_read);
int write_memory(EMU_Ram *ram, u16 address, u16 value, bool sixteen_bit_write);

int exe_push(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_pop(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_mov(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_load(Emulator *emu, EMU_Decoded_Instruction *instruction);
int exe_str(Emulator *emu, EMU_Decoded_Instruction *instruction);


#endif