#ifndef EMU_FPU__
#define EMU_FPU__

#include "my_stdtypes.h"
#include "emulator/flags.h"
#include "MemTrack.h"

enum FPU_Registers{

    FPU_ONE,
    FPU_TWO,
    FPU_THREE,
    FPU_FOUR,
    FPU_FIVE,
    FPU_SIX,
    FPU_SEVEN,
    FPU_EIGHT,
    FPU_NINE,
    FPU_TEN,
    FPU_ELEVEN,
    FPU_TWELVE,
    FPU_THIRTEEN,
    FPU_FOURTEEN,
    FPU_FIFTEEN,
    FPU_SIXTEEN,
    FPU_REG_AMOUNT

};


typedef void (*FPU_add)(f16, f16, f16*, CMP_Flags*);
typedef void (*FPU_sub)(f16, f16, f16*, CMP_Flags*);
typedef void (*FPU_mul)(f16, f16, f16*, CMP_Flags*);
typedef void (*FPU_div)(f16, f16, f16*, CMP_Flags*);
typedef void (*FPU_sqrt)(f16, f16*, CMP_Flags*);
typedef void (*FPU_cmp)(f16, f16, CMP_Flags*);
typedef void (*FPU_fint)(f16, u16*);
typedef void (*FPU_iflo)(u16, f16*);


typedef struct{

    f16 registers[FPU_REG_AMOUNT];
    FPU_add add;
    FPU_sub sub;
    FPU_mul mul;
    FPU_div div;
    FPU_sqrt sqrt;
    FPU_cmp cmp;
    FPU_fint fint;
    FPU_iflo iflo;

} EMU_Fpu;




int EMU_init_fpu(EMU_Fpu *p_fpu);


#endif