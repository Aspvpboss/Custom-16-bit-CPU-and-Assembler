#include "pipeline/execute.h"
#include "debug.h"


typedef enum{

    OP_ZERO,
    OP_ONE,
    OP_TWO

} EMU_Operands;



int exe_add(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){

        emu->alu.add(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.add(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_sub(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.sub(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.sub(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_mul(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.mul(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.mul(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_div(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.div(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.div(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_mod(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.div(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.div(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_cmp(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.cmp(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.cmp(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_and(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.and(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.and(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_nor(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.nor(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.nor(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_xor(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.xor(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]]);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.xor(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]]);
        return 0;

    }

    return 1;
}

int exe_ars(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.ars(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.ars(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_lrs(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.lrs(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.lrs(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}

int exe_lls(Emulator *emu, EMU_Decoded_Instruction *instructions){

    u16 *registers = emu->alu.registers;
    u16 *opcodes = instructions->operands;

    EMU_Addressing_Modes addr_mode = instructions->addressing_mode;

    if(addr_mode == ADDR_REG){        

        emu->alu.lls(registers[opcodes[OP_ZERO]], registers[opcodes[OP_TWO]], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    } else if(addr_mode == ADDR_REG_IMMEDIATE_EIGHT || addr_mode == ADDR_REG_IMMEDIATE_SIXTEEN){

        emu->alu.lls(registers[opcodes[OP_ZERO]], opcodes[OP_TWO], &registers[opcodes[OP_ONE]], &emu->flags);
        return 0;

    }

    return 1;
}




int execute(Emulator *emu, EMU_Decoded_Instruction *instruction){

    EMU_Instructions opcode = EMU_JMP;
    // EMU_Instructions opcode = instruction->opcode;
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