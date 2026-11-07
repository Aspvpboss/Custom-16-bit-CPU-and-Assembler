#ifndef EMU_MMIO_DATA_H
#define EMU_MMIO_DATA_H

#include <time.h>
#include "my_stdtypes.h"

typedef struct {
    struct timespec base_time;
    bool *key_buf;
    u16 num_keys;
    u16 num_curr, num_prev;
    bool vram_buf_swap_signal;
} EMU_mmio;

#endif