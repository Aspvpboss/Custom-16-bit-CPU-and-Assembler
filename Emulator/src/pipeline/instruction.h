#ifndef EMU_INSTRUCTION_H_
#define EMU_INSTRUCTION_H_

#include "my_stdtypes.h"


typedef enum{

    OP_ZERO,
    OP_ONE,
    OP_TWO

} EMU_Operands;


typedef enum{

    ADDR_REG,
    ADDR_REG_INDIRECT,
    ADDR_REG_IMMEDIATE_EIGHT,
    ADDR_REG_IMMEDIATE_SIXTEEN,
    ADDR_IMMEDIATE_EIGHT_DEST,
    ADDR_IMMEDIATE_SIXTEEN_DEST,
    ADDR_IMMEDIATE_EIGHT,
    ADDR_IMMEDIATE_SIXTEEN

} EMU_Addressing_Modes;

typedef enum{

    EMU_LOAD,
    EMU_STR,
    EMU_PUSH,
    EMU_POP,
    EMU_MOV,
    EMU_SYSCALL,
    EMU_JMP,
    EMU_JIF,
    EMU_CAL,
    EMU_CIF,
    EMU_RET,
    EMU_ADD,
    EMU_SUB,
    EMU_MUL,
    EMU_DIV,
    EMU_MOD,
    EMU_CMP,
    EMU_AND,
    EMU_NOR,
    EMU_XOR,
    EMU_ARS,
    EMU_LRS,
    EMU_LLS,
    EMU_FADD,
    EMU_FSUB,
    EMU_FMUL,
    EMU_FDIV,
    EMU_FSQRT,
    EMU_FCMP,
    EMU_FINT,
    EMU_IFLO

} EMU_Instructions;


typedef struct{

    u64 raw_instruction;
    EMU_Instructions opcode;
    EMU_Addressing_Modes addressing_mode;
    u16 operands[3];

} EMU_Decoded_Instruction;



#endif