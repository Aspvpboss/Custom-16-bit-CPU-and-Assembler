
#include "hardware/fpu.h"

#include <fenv.h>
#include <math.h>




static void fpu_add(f16 a, f16 b, f16 *dst, CMP_Flags *flags){

    feclearexcept(FE_ALL_EXCEPT);

    *dst = a + b;

    int f_flags = fetestexcept(FE_ALL_EXCEPT);

    if(f_flags & FE_ALL_EXCEPT){
        *flags |= CMP_OVER;
        *dst = 0.0f;
    }

}

static void fpu_sub(f16 a, f16 b, f16 *dst, CMP_Flags *flags){

    feclearexcept(FE_ALL_EXCEPT);

    *dst = a - b;

    int f_flags = fetestexcept(FE_ALL_EXCEPT);

    if(f_flags & (FE_UNDERFLOW | FE_OVERFLOW)){
        *flags |= CMP_OVER;
        *dst = 0.0f;
    }

}
 
static void fpu_mul(f16 a, f16 b, f16 *dst, CMP_Flags *flags){

    feclearexcept(FE_ALL_EXCEPT);

    *dst = a * b;

    int f_flags = fetestexcept(FE_ALL_EXCEPT);

    if(f_flags & FE_ALL_EXCEPT){
        *flags |= CMP_OVER;
        *dst = 0.0f;
    }

}

static void fpu_div(f16 a, f16 b, f16 *dst, CMP_Flags *flags){

    feclearexcept(FE_ALL_EXCEPT);

    *dst = a / b;

    int f_flags = fetestexcept(FE_ALL_EXCEPT);

    if(f_flags & FE_ALL_EXCEPT){
        *flags |= CMP_OVER;
        *dst = 0.0f;
    }

}

static void fpu_sqrt(f16 a, f16 *dst, CMP_Flags *flags){

    feclearexcept(FE_ALL_EXCEPT);

    *dst = sqrtf((f16)a);

    int f_flags = fetestexcept(FE_ALL_EXCEPT);

    if(f_flags & FE_ALL_EXCEPT){
        *flags |= CMP_OVER;
        *dst = 0.0f;
    }

}

static void fpu_cmp(f16 a, f16 b, CMP_Flags *flags){

    if(a < b){
        *flags |= (CMP_SLT | CMP_SLE);
    }
    if(a > b){
        *flags |= (CMP_SGR | CMP_SGE);
    }
    if(a == b){
        *flags |= (CMP_SGE | CMP_SLE | CMP_EQU);
    }
    if(a != b){
        *flags |= CMP_NEQ;
    }
    if(a == 0.0f){
        *flags |= CMP_IF0;
    }

}

static void fpu_fint(f16 input, u16 *output){
    *output = (u16)(input);
}

static void fpu_iflo(u16 input, f16 *output){
    *output = (f16)((i16)input);
}




int EMU_init_fpu(EMU_Fpu *p_fpu){

    p_fpu->add = fpu_add;
    p_fpu->sub = fpu_sub;
    p_fpu->mul = fpu_mul;
    p_fpu->div = fpu_div;
    p_fpu->sqrt = fpu_sqrt;
    p_fpu->cmp = fpu_cmp;
    p_fpu->fint = fpu_fint;
    p_fpu->iflo = fpu_iflo;
    memset(p_fpu->registers, 0, sizeof(p_fpu->registers));

    return 0;
}