#include "hardware/alu.h"



static void alu_add(u16 a, u16 b, u16 *dst, CMP_Flags *flags){
    
    u32 wide = (u32)a + (u32)b;
    u16 result = (u16)wide;
    *dst = result;

    if(wide > 0xFFFF)
        *flags |= CMP_CAR;

    if((~(a ^ b) & (a ^ result) & 0x8000) != 0)
        *flags |= CMP_OVER;

}

static void alu_sub(u16 a, u16 b, u16 *dst, CMP_Flags *flags){
    
    u32 wide = (u32)a - (u32)b;
    u16 result = (u16)wide;
    *dst = result;
    
    if(a < b)
        *flags |= CMP_CAR;

    if(((a ^ b) & (a ^ result) & 0x8000) != 0)
        *flags |= CMP_OVER;

}

static void alu_mul(u16 a, u16 b, u16 *dst, CMP_Flags *flags){
    u32 result = a * b;
    (*dst) = result;
    
    if(result > __UINT16_MAX__){
        *flags |= CMP_CAR;
    }

    i16 upper = result >> 16;
    i16 sign = (i16)result >> 16;

    if(sign != upper){
        *flags |= CMP_OVER;
    }
    
}

static void alu_div(u16 a, u16 b, u16 *dst, CMP_Flags *flags){

    if(b == 0){
        *flags |= CMP_OVER;
        (*dst) = 0;
        return;
    }

    if((i16)a == -__INT16_MAX__ && (i16)b == -1){
        *flags |= CMP_CAR;
        (*dst) = 0;
        return;
    }

    (*dst) = (u16)(a / b);

}

static void alu_mod(u16 a, u16 b, u16 *dst, CMP_Flags *flags){
    if(b == 0){
        *flags |= CMP_OVER;
        (*dst) = 0;
        return;
    }

    if((i16)a == -__INT16_MAX__ && (i16)b == -1){
        *flags |= CMP_CAR;
        (*dst) = 0;
        return;
    }


    (*dst) = (u16)(a % b);
}

static void alu_and(u16 a, u16 b, u16 *dst){
    (*dst) = a & b;
}

static void alu_nor(u16 a, u16 b, u16 *dst){
    (*dst) = ~(a | b);
}

static void alu_xor(u16 a, u16 b, u16 *dst){
    (*dst) = a ^ b;
}

static void alu_cmp(u16 a, u16 b, CMP_Flags *dst){
    
    u16 result = a - b;
    CMP_Flags t_flags = 0;

    if(a == 0){
        t_flags |= CMP_IF0;
    }

    if(result == 0){
        t_flags |= CMP_EQU;
        t_flags |= CMP_ULE;
        t_flags |= CMP_UGE;
    } else{
        t_flags |= CMP_NEQ;
    } 

    if((i16)result < 0){
        t_flags |= CMP_ULT;
        t_flags |= CMP_ULE; 
    }
    if((i16)result > 0){
        t_flags |= CMP_UGR;
        t_flags |= CMP_UGE; 
    }

    bool Z = (result == 0);
    bool N = (result & 0x8000) != 0;
    bool V = ((a ^ b) & (a ^ result) & 0x8000) != 0;

    bool signed_lt = N ^ V;
    bool signed_ge = !signed_lt;
    bool signed_le = Z || signed_lt;
    bool signed_gt = !Z && signed_ge;

    if(signed_lt)  t_flags |= CMP_SLT;
    if(signed_le)  t_flags |= CMP_SLE;
    if(signed_gt)  t_flags |= CMP_SGR;
    if(signed_ge)  t_flags |= CMP_SGE;
    (*dst) = t_flags;

}

static void alu_ars(u16 a, u16 shift, u16 *dst, CMP_Flags *flags){
    *dst = (i16)a >> shift;
}

static void alu_lrs(u16 a, u16 shift, u16 *dst, CMP_Flags *flags){
    *dst = (u16)a >> shift;
}

static void alu_lls(u16 a, u16 shift, u16 *dst, CMP_Flags *flags){
    u32 result = a << shift;
    if(result >> 16){
        *flags |= CMP_OVER;
    }

    *dst = (u16)result;
}




int EMU_init_alu(EMU_Alu *p_alu){

    p_alu->add = alu_add;
    p_alu->sub = alu_sub;
    p_alu->mul = alu_mul;
    p_alu->div = alu_div;
    p_alu->mod = alu_mod;

    p_alu->ars = alu_ars;
    p_alu->lrs = alu_lrs;
    p_alu->lls = alu_lls;

    p_alu->cmp = alu_cmp;
    p_alu->and = alu_and;
    p_alu->nor = alu_nor;
    p_alu->xor = alu_xor;

    memset(p_alu->registers, 0, sizeof(p_alu->registers));

    return 0;
}