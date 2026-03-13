#include "pipeline/execute.h"
#include "debug.h"


typedef enum{

    OP_ZERO,
    OP_ONE,
    OP_TWO

} EMU_Operands;



int exe_add(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->alu.add(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.add(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_sub(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.sub(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.sub(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_mul(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.mul(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.mul(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_div(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.div(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.div(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_mod(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.div(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.div(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_cmp(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.cmp(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.cmp(registers[operands[OP_ZERO]], operands[OP_TWO], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_and(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.and(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.and(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_nor(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.nor(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.nor(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_xor(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.xor(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.xor(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_ars(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.ars(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.ars(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_lrs(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.lrs(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.lrs(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_lls(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *registers = emu->alu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.lls(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.lls(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

exe_fadd(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.add(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.add(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

exe_fsub(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.sub(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.sub(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

exe_fmul(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.mul(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.mul(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

exe_fdiv(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.div(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.div(registers[operands[OP_ZERO]], operands[OP_TWO], &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

exe_fsqrt(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.sqrt(registers[operands[OP_ZERO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.sqrt(registers[operands[OP_ZERO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

exe_fcmp(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.cmp(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.cmp(registers[operands[OP_ZERO]], (*(f16*)&operands[OP_TWO]), &emu->flags);
        return 0;
    }

    return 1;
}

exe_fint(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *alu_registers = emu->alu.registers;
    f16 *float_registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.fint(float_registers[operands[OP_ZERO]], &alu_registers[operands[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.fint((*(f16*)&operands[OP_TWO]), operands[OP_TWO]);
        
        return 0;
    }

    return 1;
}



int execute(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Instructions opcode = EMU_JMP;
    // EMU_instruction opcode = instruction->opcode;
    // EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    switch(opcode){

        case EMU_LOAD:
            break;

        case EMU_STR:
            break;

        case EMU_PUSH:
            break;

        case EMU_POP:
            break;

        case EMU_LDI:
            break;

        case EMU_MOV:
            break;

        case EMU_SYSCALL:
            break;

        case EMU_JMP:
            break;

        case EMU_JIF:
            break;

        case EMU_CAL:
            break;

        case EMU_CIF:
            break;

        case EMU_RET:
            break;

        case EMU_ADD:
            if(exe_add(emu, instruction)) return 1;
            break;

        case EMU_SUB:
            if(exe_sub(emu, instruction)) return 1;
            break;

        case EMU_MUL:
            if(exe_mul(emu, instruction)) return 1;
            break;

        case EMU_DIV:
            if(exe_div(emu, instruction)) return 1;
            break;

        case EMU_MOD:
            if(exe_mod(emu, instruction)) return 1;
            break;

        case EMU_CMP:
            if(exe_cmp(emu, instruction)) return 1;
            break;

        case EMU_AND:
            if(exe_and(emu, instruction)) return 1;
            break;

        case EMU_NOR:
            if(exe_nor(emu, instruction)) return 1;
            break;

        case EMU_XOR:
            if(exe_xor(emu, instruction)) return 1;
            break;

        case EMU_ARS:
            if(exe_ars(emu, instruction)) return 1;
            break;

        case EMU_LRS:
            if(exe_lrs(emu, instruction)) return 1;
            break;

        case EMU_LLS:
            if(exe_lls(emu, instruction)) return 1;
            break;

        case EMU_FADD:
            break;

        case EMU_FSUB:
            break;

        case EMU_FMUL:
            break;

        case EMU_FDIV:
            break;

        case EMU_FSQRT:
            break;

        case EMU_FCMP:
            break;

        case EMU_FINT:
            break;

        case EMU_IFLO:
            break;

        default:
            break;
    }

    return 0;
}