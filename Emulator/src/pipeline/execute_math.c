#include "pipeline/execute_math.h"


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

int exe_fadd(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.add(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.add(registers[operands[OP_ZERO]], (*(f16*)&operands[OP_TWO]), &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

int exe_fsub(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.sub(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.sub(registers[operands[OP_ZERO]], (*(f16*)&operands[OP_TWO]), &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

int exe_fmul(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.mul(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.mul(registers[operands[OP_ZERO]], (*(f16*)&operands[OP_TWO]), &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

int exe_fdiv(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.div(registers[operands[OP_ZERO]], registers[operands[OP_TWO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.div(registers[operands[OP_ZERO]], (*(f16*)&operands[OP_TWO]), &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

int exe_fsqrt(Emulator *emu, EMU_Decoded_Instruction *instruction){

    f16 *registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.sqrt(registers[operands[OP_ZERO]], &registers[operands[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->fpu.sqrt((*(f16*)&operands[OP_TWO]), &registers[operands[OP_ONE]], &emu->flags);
        return 0;
    }

    return 1;
}

int exe_fcmp(Emulator *emu, EMU_Decoded_Instruction *instruction){

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

int exe_fint(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *alu_registers = emu->alu.registers;
    f16 *float_registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.fint(float_registers[operands[OP_ZERO]], &alu_registers[operands[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_iflo(Emulator *emu, EMU_Decoded_Instruction *instruction){

    u16 *alu_registers = emu->alu.registers;
    f16 *float_registers = emu->fpu.registers;
    u16 *operands = instruction->operands;

    EMU_Addressing_Modes addr_mode = instruction->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->fpu.iflo(alu_registers[operands[OP_ZERO]], &float_registers[operands[OP_ONE]]);
        return 0;

    }

    return 1;
}