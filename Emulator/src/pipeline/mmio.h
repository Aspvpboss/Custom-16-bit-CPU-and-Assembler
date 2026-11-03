#ifndef EMU_MMIO_H_
#define EMU_MMIO_H_

#include <time.h>
#include "pipeline/mmio_data.h"
#include "hardware/emulator.h"


int write_mmio(Emulator *emu, u16 address, u16 write_value);
int read_mmio(Emulator *emu, u16 address, u16 *read_value);
int mmio(Emulator *emu);

#endif