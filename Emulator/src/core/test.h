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
    #define PRINT_CMP_FLAGS(flags)
    #define PRINT_INDIVIDUAL_BYTES(value)
#else
    #define d_printf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
    #define RUN_TESTS() run_tests()
    #define PRINT_CMP_FLAGS(flags) print_cmp_flags(flags)
    #define PRINT_INDIVIDUAL_BYTES(value) print_individual_bytes(value)
#endif

#endif