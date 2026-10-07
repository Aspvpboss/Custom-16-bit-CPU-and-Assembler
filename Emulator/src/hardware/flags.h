#ifndef EMU_FLAGS_H_
#define EMU_FLAGS_H_

#include "my_stdtypes.h"

enum CMP_FlagMasks{

    CMP_EQU = 1 << 0,
    CMP_NEQ = 1 << 1,
    CMP_IF0 = 1 << 2,
    
    CMP_ULT = 1 << 3,
    CMP_ULE = 1 << 4,
    CMP_UGR = 1 << 5,
    CMP_UGE = 1 << 6,
    
    CMP_SLT = 1 << 7,
    CMP_SLE = 1 << 8,
    CMP_SGR = 1 << 9,
    CMP_SGE = 1 << 10,
    
    CMP_CAR = 1 << 11,
    CMP_OVER = 1 << 12,

};

typedef u16 CMP_Flags;

#endif