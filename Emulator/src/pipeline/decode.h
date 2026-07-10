#ifndef EMU_DECODE_H_
#define EMU_DECODE_H_

#include "pipeline/instruction.h"
#include "my_stdtypes.h"
#include "emulator/emulator.h"

int decode(Emulator *emu, EMU_Decoded_Instruction *instruction);

#endif