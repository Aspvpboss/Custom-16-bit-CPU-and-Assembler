#ifndef EMU_DEBUG_H_
#define EMU_DEBUG_H_

#include "hardware/flags.h"
#include "hardware/emulator.h"

void print_cmp_flags(CMP_Flags flags);
void print_individual_bytes(u64 value);
int run_tests();


#ifdef NDEBUG
    #define d_printf(fmt, ...) 
    #define RUN_TESTS()
#else
    #define d_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #define RUN_TESTS() run_tests()
#endif

#endif