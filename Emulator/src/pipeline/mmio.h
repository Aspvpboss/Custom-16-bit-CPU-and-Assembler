#ifndef EMU_MMIO_H_
#define EMU_MMIO_H_

#include <time.h>
#include "hardware/emulator.h"


typedef struct {
    bool vram_buf_swap_signal;
    struct timespec base_time, current_time;
} EMU_mmio;

int mmio(Emulator *emu);
int write_mmio(Emulator *emu, u16 address, u16 write_value);
int read_mmio(Emulator *emu, u16 address, u16 *read_value);

#endif