#ifndef EMU_MMIO_DATA_H
#define EMU_MMIO_DATA_H

#include <time.h>
#include "my_stdtypes.h"

typedef struct {
    bool vram_buf_swap_signal;
    struct timespec base_time, current_time;
} EMU_mmio;

#endif