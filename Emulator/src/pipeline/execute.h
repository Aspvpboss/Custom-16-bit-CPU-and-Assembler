#ifndef EMU_EXECUTE_H_
#define EMU_EXECUTE_H_

#include "emulator/emulator.h"
#include "pipeline/instruction.h"


int execute(Emulator *emu, EMU_Decoded_Instruction *instruction);

#endif