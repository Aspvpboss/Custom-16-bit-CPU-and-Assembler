#ifndef EMU_FETCH_H_
#define EMU_FETCH_H_

#include "my_stdtypes.h"
#include "emulator/emulator.h"
#include "pipeline/instruction.h"

int fetch(Emulator *emu, EMU_Decoded_Instruction *instruction);

#endif