#ifndef EMU_ALU__
#define EMU_ALU__

#include "my_stdtypes.h"
#include <SDLite/MemTrack.h>
#include "emulator/flags.h"

enum ALU_Registers{

    ALU_ZERO,
    ALU_ONE,
    ALU_TWO,
    ALU_THREE,
    ALU_FOUR,
    ALU_FIVE,
    ALU_SIX,
    ALU_SEVEN,
    ALU_EIGHT,
    ALU_NINE,
    ALU_TEN,
    ALU_ELEVEN,
    ALU_ROM_BANK,
    ALU_FRAME_POINTER,
    ALU_STACK_POINTER,
    ALU_PROGRAM_COUNTER,
    ALU_REG_AMOUNT

};




// ALU functions

typedef void (*ALU_add)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_sub)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_mul)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_div)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_mod)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_ars)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_lrs)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_lls)(u16, u16, u16*, CMP_Flags*);
typedef void (*ALU_cmp)(u16, u16, CMP_Flags*);
typedef void (*ALU_and)(u16, u16, u16*);
typedef void (*ALU_nor)(u16, u16, u16*);
typedef void (*ALU_xor)(u16, u16, u16*);


typedef struct{
 
    u16 registers[ALU_REG_AMOUNT];
    ALU_add add;
    ALU_sub sub;
    ALU_mul mul;
    ALU_div div;
    ALU_mod mod;
    ALU_ars ars;
    ALU_lrs lrs;
    ALU_lls lls;
    ALU_cmp cmp;
    ALU_and and;
    ALU_nor nor;
    ALU_xor xor;

} EMU_Alu;




int EMU_init_alu(EMU_Alu *p_alu);


#endif