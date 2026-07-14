#ifndef EMU_DEBUG_H_
#define EMU_DEBUG_H_

#include "emulator/flags.h"
#include "emulator/emulator.h"

void print_cmp_flags(CMP_Flags flags);
void print_individual_bytes(u64 value);
int memory_test(EMU_Ram *ram);


#ifdef NDEBUG
    #define d_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define d_printf(fmt, ...)
#endif

#endif