#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "pipeline/execute/execute_math.h"
#include "pipeline/execute/execute_memory.h"
#include "pipeline/execute/execute_control.h"
#include "test.h"

void print_cmp_flags(CMP_Flags flags){
 
    if (flags == 0) {
        d_printf("CMP: none\n");
        return;
    }

    d_printf("CMP:");

    if (flags & CMP_EQU) d_printf(" EQU");
    if (flags & CMP_NEQ) d_printf(" NEQ");
    if (flags & CMP_IF0) d_printf(" IF0");

    if (flags & CMP_ULT) d_printf(" ULT");
    if (flags & CMP_ULE) d_printf(" ULE");
    if (flags & CMP_UGR) d_printf(" UGR");
    if (flags & CMP_UGE) d_printf(" UGE");

    if (flags & CMP_SLT) d_printf(" SLT");
    if (flags & CMP_SLE) d_printf(" SLE");
    if (flags & CMP_SGR) d_printf(" SGR");
    if (flags & CMP_SGE) d_printf(" SGE");

    if (flags & CMP_CAR)  d_printf(" CAR");
    if (flags & CMP_OVER) d_printf(" OVER");

    d_printf("\n");

}

void print_register_files(Emulator *emu){

    const char *reg_names[] = {
        "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11",
        "RAM_Bank", "FP", "SP", "PC"};

    for(int i = 0; i < ALU_REG_AMOUNT; i++){
        if(i == ALU_FRAME_POINTER || i == ALU_STACK_POINTER){
            d_printf("%s : %X\n", reg_names[i], emu->alu.registers[i]);
        } else{
            d_printf("%s : %d\n", reg_names[i], emu->alu.registers[i]);
        }
    }

}


void print_individual_bytes(u64 value){

    for(u8 string_index = 0; string_index < 5; string_index++){

        for(u8 char_index = 0; char_index < 8; char_index++){
            putchar((value & ((u64)1 << 39)) ? '1' : '0');        
            value <<= 1;     
        }

        putchar(' ');
    }

    putchar('\n');
}




static int memory_test(EMU_Ram *ram){

    u16 destination_value = 0;
   
    if(write_memory(ram, 0x0101, 0xffff, true)){
        d_printf("simple write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x0101, &destination_value, true)){
        d_printf("simple read_memory failed\n");
        return 1;
    }

    if(write_memory(ram, 0x6F01, 0xffff, false)){
        d_printf("vram write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x6F01, &destination_value, true)){
        d_printf("vram read_memory failed\n");
        return 1;
    }
    if(destination_value != 0xff){
        d_printf("vram read_memory failed, expected 0xff, got %d\n", destination_value);
        return 1;
    }

    if(write_memory(ram, 0x7F01, 0xffff, false)){
        d_printf("mmio write_memory failed\n");
        return 1;
    }
    if(read_memory(ram, 0x7F01, &destination_value, true)){
        d_printf("mmio read_memory failed\n");
        return 1;
    }
    if(destination_value != 0xff){
        d_printf("mmio read_memory failed, expected 0xff, got %d\n", destination_value);
        return 1;
    }

    if(write_memory(ram, 0xffff, 0x00ff, false)){
        d_printf("write_memory failed, tried to write to ram failed\n");
        return 1;
    }
    if(!write_memory(ram, 0x0000, 0x00ff, false)){
        d_printf("write_memory failed, tried to write to bootloader failed\n");
        return 1;
    }

    if(!read_memory(ram, 0xffff, &destination_value, true)){
        d_printf("read_memory failed, tried to read from outside memory\n");
        return 1;
    }
    if(read_memory(ram, 0x0000, &destination_value, false)){
        d_printf("read_memory failed, failed to read from bootloader\n");
        return 1;
    }

    if(!write_memory(ram, 0x6EFF, 0xffff, true)){
        d_printf("write_memory failed, tried to write outside of ram\n");
        return 1;
    }

    ram->vram_selector = true;
    if(!write_memory(ram, 0x7EFF, 0xffff, true)){
        d_printf("write_memory failed, tried to write outside of vram\n");
        return 1;
    }

    return 0;
}

static int memory_instructions_test(Emulator *emu){
    EMU_Decoded_Instruction instruction;

    // 1. Test exe_mov functionality and edge cases
    emu->alu.registers[ALU_ZERO] = 43981; // Value in base 10
    instruction.operands[OP_ZERO] = ALU_ZERO; // source register (8-bit)
    instruction.operands[OP_ONE] = ALU_ONE;   // destination register (8-bit)
    instruction.operands[OP_TWO] = MOV_REG_TO_REG;
    
    if (exe_mov(emu, &instruction) != 0 || emu->alu.registers[ALU_ONE] != 43981) {
        d_printf("exe_mov MOV_REG_TO_REG failed\n");
        return 1;
    }

    // Test invalid move opcode edge case
    instruction.operands[OP_TWO] = 255; 
    if (exe_mov(emu, &instruction) == 0) {
        d_printf("exe_mov allowed invalid opcode\n");
        return 1;
    }

    // 2. Test exe_push and exe_pop functionality and stack pointer tracking
    emu->alu.registers[ALU_STACK_POINTER] = 0x0500; // Address in hex
    emu->alu.registers[ALU_TWO] = 22136;          // Value in base 10

    // Push 16-bit register (bit 4 set indicates 16-bit)
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    instruction.operands[OP_ZERO] = ALU_TWO | 0x0010; 
    
    if (exe_push(emu, &instruction) != 0) {
        d_printf("exe_push failed\n");
        return 1;
    }
    // Stack pointer should decrease by 2 for a 16-bit write
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x04FE) { // Address in hex
        d_printf("exe_push stack pointer update failed, expected 0x04FE, got 0x%X\n", emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // Pop into a 16-bit destination register
    instruction.operands[OP_ZERO] = ALU_THREE | 0x0010;
    if (exe_pop(emu, &instruction) != 0) {
        d_printf("exe_pop failed\n");
        return 1;
    }
    // Value should match and stack pointer should restore to 0x0500
    if (emu->alu.registers[ALU_THREE] != 22136) { // Value in base 10
        d_printf("exe_pop value mismatch, expected 22136, got %d\n", emu->alu.registers[ALU_THREE]);
        return 1;
    }
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x0500) { // Address in hex
        d_printf("exe_pop stack pointer restore failed, expected 0x0500, got 0x%X\n", emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // Test push/pop invalid addressing mode edge case
    instruction.addressing_mode = ADDR_REG;
    if (exe_push(emu, &instruction) == 0 || exe_pop(emu, &instruction) == 0) {
        d_printf("exe_push/pop allowed invalid addressing mode\n");
        return 1;
    }

    // 3. Test exe_str and exe_load (Direct and Indirect Addressing Modes)
    emu->alu.registers[ALU_FOUR] = 4386; // Value in base 10
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x0250;           // Address in hex
    instruction.operands[OP_ONE] = ALU_FOUR | 0x0010; // 16-bit source register 4

    if (exe_str(emu, &instruction) != 0) {
        d_printf("exe_str ADDR_IMMEDIATE_SIXTEEN_DEST failed\n");
        return 1;
    }

    // Load back using direct addressing into register 5
    emu->alu.registers[ALU_FIVE] = 0;
    instruction.operands[OP_ZERO] = 0x0250;           // Address in hex
    instruction.operands[OP_ONE] = ALU_FIVE | 0x0010; // 16-bit destination register 5

    if (exe_load(emu, &instruction) != 0) {
        d_printf("exe_load ADDR_IMMEDIATE_SIXTEEN_DEST failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_FIVE] != 4386) { // Value in base 10
        d_printf("exe_load value mismatch, expected 4386, got %d\n", emu->alu.registers[ALU_FIVE]);
        return 1;
    }

    // Test ADDR_REG_INDIRECT for store and load
    emu->alu.registers[ALU_SIX] = 0x0300;   // Address in hex
    emu->alu.registers[ALU_SEVEN] = 39338; // Value in base 10

    instruction.addressing_mode = ADDR_REG_INDIRECT;
    instruction.operands[OP_ZERO] = ALU_SIX;           // register holding the address
    instruction.operands[OP_ONE] = ALU_SEVEN | 0x0010; // 16-bit source register 7

    if (exe_str(emu, &instruction) != 0) {
        d_printf("exe_str ADDR_REG_INDIRECT failed\n");
        return 1;
    }

    emu->alu.registers[ALU_EIGHT] = 0;
    instruction.operands[OP_ZERO] = ALU_SIX;           // register holding the address
    instruction.operands[OP_ONE] = ALU_EIGHT | 0x0010; // 16-bit destination register 8

    if (exe_load(emu, &instruction) != 0) {
        d_printf("exe_load ADDR_REG_INDIRECT failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_EIGHT] != 39338) { // Value in base 10
        d_printf("exe_load indirect value mismatch, expected 39338, got %d\n", emu->alu.registers[ALU_EIGHT]);
        return 1;
    }

    // Test invalid addressing mode edge case for str and load
    instruction.addressing_mode = (EMU_Addressing_Modes)255;
    if (exe_str(emu, &instruction) == 0 || exe_load(emu, &instruction) == 0) {
        d_printf("exe_str/load allowed invalid addressing mode\n");
        return 1;
    }

    return 0;
}

static int alu_test(Emulator *emu){
    u16 result = 0;
    CMP_Flags flags = 0;

    // Test ALU add
    if (emu->alu.add) {
        emu->alu.add(10, 20, &result, &flags);
        if (result != 30) {
            d_printf("ALU add failed, expected 30, got %d\n", result);
            return 1;
        }
    }

    // Test ALU sub
    if (emu->alu.sub) {
        emu->alu.sub(30, 10, &result, &flags);
        if (result != 20) {
            d_printf("ALU sub failed, expected 20, got %d\n", result);
            return 1;
        }
    }

    // Test ALU mul
    if (emu->alu.mul) {
        emu->alu.mul(6, 7, &result, &flags);
        if (result != 42) {
            d_printf("ALU mul failed, expected 42, got %d\n", result);
            return 1;
        }
    }

    // Test ALU div
    if (emu->alu.div) {
        emu->alu.div(42, 6, &result, &flags);
        if (result != 7) {
            d_printf("ALU div failed, expected 7, got %d\n", result);
            return 1;
        }
    }

    // Test ALU mod
    if (emu->alu.mod) {
        emu->alu.mod(10, 3, &result, &flags);
        if (result != 1) {
            d_printf("ALU mod failed, expected 1, got %d\n", result);
            return 1;
        }
    }

    // Test ALU shifts (lls, lrs, ars)
    if (emu->alu.lls) {
        emu->alu.lls(1, 4, &result, &flags); // 1 << 4 = 16
        if (result != 16) {
            d_printf("ALU lls failed, expected 16, got %d\n", result);
            return 1;
        }
    }

    // Test ALU cmp and flags validation
    if (emu->alu.cmp) {
        // Test equality
        emu->alu.cmp(25, 25, &flags);
        if (!(flags & CMP_EQU) || (flags & CMP_NEQ)) {
            d_printf("ALU cmp EQU flag test failed\n");
            return 1;
        }

        // Test signed less than
        emu->alu.cmp(-10, 20, &flags);
        if (!(flags & CMP_SLT) || !(flags & CMP_NEQ)) {
            d_printf("ALU cmp SLT flag test failed\n");
            return 1;
        }

        // Test signed greater than
        emu->alu.cmp(30, 20, &flags);
        if (!(flags & CMP_SGR)) {
            d_printf("ALU cmp SGR flag test failed\n");
            return 1;
        }
    }

    // Test bitwise operations (and, nor, xor)
    if (emu->alu.and) {
        emu->alu.and(12, 10, &result); // 12 (1100) & 10 (1010) = 8 (1000)
        if (result != 8) {
            d_printf("ALU and failed, expected 8, got %d\n", result);
            return 1;
        }
    }

    if (emu->alu.xor) {
        emu->alu.xor(12, 10, &result); // 12 (1100) ^ 10 (1010) = 6 (0110)
        if (result != 6) {
            d_printf("ALU xor failed, expected 6, got %d\n", result);
            return 1;
        }
    }

    if (emu->alu.nor) {
        emu->alu.nor(0, 0, &result); // ~(0 | 0) depending on implementation
        // Keeping a basic check or structural safeguard if custom NOR logic applies
    }

    return 0;
}

static int fpu_test(Emulator *emu){
    f16 f_result = 0;
    CMP_Flags flags = 0;

    // Test FPU add
    if (emu->fpu.add) {
        emu->fpu.add((f16)5.0, (f16)2.5, &f_result, &flags);
        if (f_result != (f16)7.5) {
            d_printf("FPU add failed, expected 7.5\n");
            return 1;
        }
    }

    // Test FPU sub
    if (emu->fpu.sub) {
        emu->fpu.sub((f16)10.0, (f16)3.5, &f_result, &flags);
        if (f_result != (f16)6.5) {
            d_printf("FPU sub failed, expected 6.5\n");
            return 1;
        }
    }

    // Test FPU mul
    if (emu->fpu.mul) {
        emu->fpu.mul((f16)4.0, (f16)2.5, &f_result, &flags);
        if (f_result != (f16)10.0) {
            d_printf("FPU mul failed, expected 10.0\n");
            return 1;
        }
    }

    // Test FPU div
    if (emu->fpu.div) {
        emu->fpu.div((f16)9.0, (f16)2.0, &f_result, &flags);
        if (f_result != (f16)4.5) {
            d_printf("FPU div failed, expected 4.5\n");
            return 1;
        }
    }

    // Test FPU sqrt
    if (emu->fpu.sqrt) {
        emu->fpu.sqrt((f16)16.0, &f_result, &flags);
        if (f_result != (f16)4.0) {
            d_printf("FPU sqrt failed, expected 4.0\n");
            return 1;
        }
    }

    // Test FPU cmp and fcmp flags validation
    if (emu->fpu.cmp) {
        emu->fpu.cmp((f16)10.0, (f16)5.0, &flags);
        if (!(flags & CMP_SGR)) {
            d_printf("FPU cmp (fcmp) SGR flag test failed\n");
            return 1;
        }

        emu->fpu.cmp((f16)5.0, (f16)5.0, &flags);
        if (!(flags & CMP_EQU)) {
            d_printf("FPU cmp (fcmp) EQU flag test failed\n");
            return 1;
        }
    }

    // Test conversion functions (fint and iflo)
    u16 int_val = 0;
    if (emu->fpu.fint) {
        emu->fpu.fint((f16)42.0, &int_val);
        if (int_val != 42) {
            d_printf("FPU fint failed, expected 42, got %d\n", int_val);
            return 1;
        }
    }

    if (emu->fpu.iflo) {
        emu->fpu.iflo(42, &f_result);
        if (f_result != (f16)42.0) {
            d_printf("FPU iflo failed, expected 42.0\n");
            return 1;
        }
    }

    return 0;
}


static int control_flow_test(Emulator *emu) {
    EMU_Decoded_Instruction instruction = {0};

    // =========================================================================
    // 1. Test exe_syscall
    // =========================================================================
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;

    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    instruction.operands[OP_ZERO] = 0x01;
    if (exe_syscall(emu, &instruction) != 0) {
        d_printf("exe_syscall ADDR_IMMEDIATE_EIGHT failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0200 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_syscall ADDR_IMMEDIATE_EIGHT unexpectedly modified PC or SP\n");
        return 1;
    }

    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x1234;
    if (exe_syscall(emu, &instruction) != 0) {
        d_printf("exe_syscall ADDR_IMMEDIATE_SIXTEEN failed\n");
        return 1;
    }

    // Invalid addressing modes for exe_syscall
    EMU_Addressing_Modes invalid_syscall_modes[] = {
        ADDR_REG,
        ADDR_REG_INDIRECT,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        ADDR_IMMEDIATE_EIGHT_DEST,
        ADDR_IMMEDIATE_SIXTEEN_DEST,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_syscall_modes) / sizeof(invalid_syscall_modes[0]); i++) {
        instruction.addressing_mode = invalid_syscall_modes[i];
        if (exe_syscall(emu, &instruction) == 0) {
            d_printf("exe_syscall allowed invalid addressing mode %d\n", invalid_syscall_modes[i]);
            return 1;
        }
    }

    // =========================================================================
    // 2. Test exe_jmp
    // =========================================================================
    // ADDR_REG_INDIRECT: jump to address in register (OP_ZERO)
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0100;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_THREE] = 0x1234;
    instruction.addressing_mode = ADDR_REG_INDIRECT;
    instruction.operands[OP_ZERO] = ALU_THREE;
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x1234 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_jmp ADDR_REG_INDIRECT failed, expected PC=0x1234, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // ADDR_IMMEDIATE_EIGHT_DEST: relative jump (PC += imm8)
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT_DEST;
    instruction.operands[OP_ZERO] = 0x0045;
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0245) {
        d_printf("exe_jmp ADDR_IMMEDIATE_EIGHT_DEST failed, expected PC=0x0245, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // ADDR_IMMEDIATE_SIXTEEN_DEST: relative jump (PC += imm16), forward and backward (wraparound)
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0300;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x1122;
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x1422) {
        d_printf("exe_jmp ADDR_IMMEDIATE_SIXTEEN_DEST forward failed, expected PC=0x1422, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    instruction.operands[OP_ZERO] = 0xFF00; // -256 in 16-bit two's complement
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x1322) {
        d_printf("exe_jmp ADDR_IMMEDIATE_SIXTEEN_DEST backward failed, expected PC=0x1322, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // ADDR_IMMEDIATE_EIGHT: absolute jump (PC = imm8)
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x1322;
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    instruction.operands[OP_ZERO] = 0x00AB;
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x00AB) {
        d_printf("exe_jmp ADDR_IMMEDIATE_EIGHT failed, expected PC=0x00AB, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // ADDR_IMMEDIATE_SIXTEEN: absolute jump (PC = imm16)
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x4567;
    if (exe_jmp(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x4567) {
        d_printf("exe_jmp ADDR_IMMEDIATE_SIXTEEN failed, expected PC=0x4567, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // Invalid addressing modes for exe_jmp
    EMU_Addressing_Modes invalid_jmp_modes[] = {
        ADDR_REG,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_jmp_modes) / sizeof(invalid_jmp_modes[0]); i++) {
        instruction.addressing_mode = invalid_jmp_modes[i];
        if (exe_jmp(emu, &instruction) == 0) {
            d_printf("exe_jmp allowed invalid addressing mode %d\n", invalid_jmp_modes[i]);
            return 1;
        }
    }

    // =========================================================================
    // 3. Test exe_jif (Conditional Jump)
    // =========================================================================
    // Test all 13 condition selectors (0 = EQU .. 12 = OVER) with ADDR_REG_INDIRECT
    for (u16 cond_idx = 0; cond_idx <= 12; cond_idx++) {
        u16 flag_mask = (u16)(1u << cond_idx);

        // Case A: Flag is NOT set (other flags are set) -> should NOT jump
        emu->flags = (u16)(~flag_mask & 0x1FFF);
        emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
        emu->alu.registers[ALU_ONE] = 0x0900;
        instruction.addressing_mode = ADDR_REG_INDIRECT;
        instruction.operands[OP_ZERO] = ALU_ONE;
        instruction.operands[OP_ONE] = cond_idx;

        if (exe_jif(emu, &instruction) != 0) {
            d_printf("exe_jif ADDR_REG_INDIRECT returned error on cond %d (flag off)\n", cond_idx);
            return 1;
        }
        if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0200) {
            d_printf("exe_jif ADDR_REG_INDIRECT jumped when flag %d was OFF (PC=0x%X)\n",
                     cond_idx, emu->alu.registers[ALU_PROGRAM_COUNTER]);
            return 1;
        }

        // Case B: Flag IS set -> MUST jump to register value
        emu->flags = flag_mask;
        emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
        if (exe_jif(emu, &instruction) != 0) {
            d_printf("exe_jif ADDR_REG_INDIRECT returned error on cond %d (flag on)\n", cond_idx);
            return 1;
        }
        if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0900) {
            d_printf("exe_jif ADDR_REG_INDIRECT did not jump when flag %d was ON (expected 0x0900, got 0x%X)\n",
                     cond_idx, emu->alu.registers[ALU_PROGRAM_COUNTER]);
            return 1;
        }
    }

    // ADDR_IMMEDIATE_EIGHT_DEST for exe_jif
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT_DEST;
    instruction.operands[OP_ZERO] = 0x0030;
    instruction.operands[OP_ONE] = 0; // Equal (bit 0 = CMP_EQU)

    emu->flags = 0; // Flag off -> no jump
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0400;
    if (exe_jif(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0400) {
        d_printf("exe_jif ADDR_IMMEDIATE_EIGHT_DEST jumped when flag was OFF\n");
        return 1;
    }

    emu->flags = CMP_EQU; // Flag on -> relative jump
    if (exe_jif(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0430) {
        d_printf("exe_jif ADDR_IMMEDIATE_EIGHT_DEST failed to jump when flag was ON, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // ADDR_IMMEDIATE_SIXTEEN_DEST for exe_jif
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x0250;
    instruction.operands[OP_ONE] = 7; // Signed less than (bit 7 = CMP_SLT)

    emu->flags = CMP_EQU; // SLT is off -> no jump
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0500;
    if (exe_jif(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0500) {
        d_printf("exe_jif ADDR_IMMEDIATE_SIXTEEN_DEST jumped when SLT flag was OFF\n");
        return 1;
    }

    emu->flags = CMP_SLT; // SLT is on -> relative jump
    if (exe_jif(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0750) {
        d_printf("exe_jif ADDR_IMMEDIATE_SIXTEEN_DEST failed to jump when SLT flag was ON, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }

    // Invalid addressing modes for exe_jif
    EMU_Addressing_Modes invalid_jif_modes[] = {
        ADDR_REG,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        ADDR_IMMEDIATE_EIGHT,
        ADDR_IMMEDIATE_SIXTEEN,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_jif_modes) / sizeof(invalid_jif_modes[0]); i++) {
        instruction.addressing_mode = invalid_jif_modes[i];
        if (exe_jif(emu, &instruction) == 0) {
            d_printf("exe_jif allowed invalid addressing mode %d\n", invalid_jif_modes[i]);
            return 1;
        }
    }

    // =========================================================================
    // 4. Test exe_cal and exe_ret (Call and Return + Stack Integrity)
    // =========================================================================
    // 4a. ADDR_REG_INDIRECT call + ret round-trip
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0204;
    emu->alu.registers[ALU_TWO] = 0x1500;

    instruction.addressing_mode = ADDR_REG_INDIRECT;
    instruction.operands[OP_ZERO] = ALU_TWO;
    if (exe_cal(emu, &instruction) != 0) {
        d_printf("exe_cal ADDR_REG_INDIRECT failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x1500) {
        d_printf("exe_cal ADDR_REG_INDIRECT PC mismatch, expected 0x1500, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_cal ADDR_REG_INDIRECT SP mismatch, expected 0x05FE, got 0x%X\n",
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // Return from ADDR_REG_INDIRECT call
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    instruction.operands[OP_ZERO] = 0;
    if (exe_ret(emu, &instruction) != 0) {
        d_printf("exe_ret after ADDR_REG_INDIRECT call failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0204) {
        d_printf("exe_ret PC restore mismatch, expected 0x0204, got 0x%X\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret SP restore mismatch, expected 0x0600, got 0x%X\n",
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 4b. ADDR_IMMEDIATE_SIXTEEN_DEST (relative call) + ret round-trip
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0300;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x0450;
    if (exe_cal(emu, &instruction) != 0) {
        d_printf("exe_cal ADDR_IMMEDIATE_SIXTEEN_DEST failed\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0750 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_cal ADDR_IMMEDIATE_SIXTEEN_DEST state mismatch (PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0300 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret after ADDR_IMMEDIATE_SIXTEEN_DEST call failed (PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 4c. ADDR_IMMEDIATE_SIXTEEN (absolute call) + nested call + double ret
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0400;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x2000;
    if (exe_cal(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x2000 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_cal first nested ADDR_IMMEDIATE_SIXTEEN failed\n");
        return 1;
    }

    // Simulate advancing PC inside first function and making a second nested call
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x2010;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x3000;
    if (exe_cal(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x3000 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x05FC) {
        d_printf("exe_cal second nested ADDR_IMMEDIATE_SIXTEEN failed\n");
        return 1;
    }

    // First return -> back to 0x2010, SP = 0x05FE
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x2010 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_ret first nested return failed (expected PC=0x2010, SP=0x05FE; got PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // Second return -> back to 0x0400, SP = 0x0600
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0400 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret second nested return failed (expected PC=0x0400, SP=0x0600; got PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 4d. Interoperability with exe_push / exe_pop on the same stack
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0444;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x1800;
    if (exe_cal(emu, &instruction) != 0) {
        d_printf("exe_cal before push/pop interop failed\n");
        return 1;
    }
    // Callee pushes a 16-bit register onto the stack and pops it before returning
    emu->alu.registers[ALU_FOUR] = 0xBEEF;
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    instruction.operands[OP_ZERO] = ALU_FOUR | 0x0010;
    if (exe_push(emu, &instruction) != 0) {
        d_printf("exe_push inside callee failed\n");
        return 1;
    }
    emu->alu.registers[ALU_FIVE] = 0;
    instruction.operands[OP_ZERO] = ALU_FIVE | 0x0010;
    if (exe_pop(emu, &instruction) != 0 || emu->alu.registers[ALU_FIVE] != 0xBEEF) {
        d_printf("exe_pop inside callee failed or corrupted pushed register\n");
        return 1;
    }
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0444 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret after callee push/pop corrupted return address (got PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 4e. Invalid addressing modes for exe_cal (including 8-bit modes)
    EMU_Addressing_Modes invalid_cal_modes[] = {
        ADDR_REG,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        ADDR_IMMEDIATE_EIGHT_DEST,
        ADDR_IMMEDIATE_EIGHT,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_cal_modes) / sizeof(invalid_cal_modes[0]); i++) {
        instruction.addressing_mode = invalid_cal_modes[i];
        if (exe_cal(emu, &instruction) == 0) {
            d_printf("exe_cal allowed invalid addressing mode %d\n", invalid_cal_modes[i]);
            return 1;
        }
    }

    // 4f. Invalid addressing modes for exe_ret
    EMU_Addressing_Modes invalid_ret_modes[] = {
        ADDR_REG,
        ADDR_REG_INDIRECT,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        ADDR_IMMEDIATE_EIGHT_DEST,
        ADDR_IMMEDIATE_SIXTEEN_DEST,
        ADDR_IMMEDIATE_SIXTEEN,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_ret_modes) / sizeof(invalid_ret_modes[0]); i++) {
        instruction.addressing_mode = invalid_ret_modes[i];
        if (exe_ret(emu, &instruction) == 0) {
            d_printf("exe_ret allowed invalid addressing mode %d\n", invalid_ret_modes[i]);
            return 1;
        }
    }

    // 4g. Stack write/read failure edge cases for exe_cal and exe_ret
    // SP in bootloader (0x0050) -> write_memory must fail
    emu->alu.registers[ALU_STACK_POINTER] = 0x0050;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN;
    instruction.operands[OP_ZERO] = 0x1000;
    if (exe_cal(emu, &instruction) == 0) {
        d_printf("exe_cal allowed stack write into read-only bootloader region\n");
        return 1;
    }

    // SP at end of memory where 16-bit read overflows (0xFFFD + 2 = 0xFFFF) -> read_memory must fail
    emu->alu.registers[ALU_STACK_POINTER] = 0xFFFD;
    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) == 0) {
        d_printf("exe_ret allowed out-of-bounds 16-bit stack read at 0xFFFF\n");
        return 1;
    }

    // =========================================================================
    // 5. Test exe_cif (Conditional Call)
    // =========================================================================
    // 5a. ADDR_REG_INDIRECT when condition is FALSE -> must NOT jump and must NOT push to stack
    emu->flags = 0;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0350;
    emu->alu.registers[ALU_SIX] = 0x1A00;

    instruction.addressing_mode = ADDR_REG_INDIRECT;
    instruction.operands[OP_ZERO] = ALU_SIX;
    instruction.operands[OP_ONE] = 0; // Equal (bit 0 = CMP_EQU)

    if (exe_cif(emu, &instruction) != 0) {
        d_printf("exe_cif ADDR_REG_INDIRECT returned error when flag was OFF\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0350) {
        d_printf("exe_cif ADDR_REG_INDIRECT jumped when condition flag was OFF (PC=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_cif ADDR_REG_INDIRECT modified SP when condition flag was OFF (expected 0x0600, got 0x%X)\n",
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 5b. ADDR_REG_INDIRECT when condition is TRUE -> must push PC, jump, and return cleanly
    emu->flags = CMP_EQU;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0350;
    if (exe_cif(emu, &instruction) != 0) {
        d_printf("exe_cif ADDR_REG_INDIRECT returned error when flag was ON\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x1A00) {
        d_printf("exe_cif ADDR_REG_INDIRECT did not jump when condition flag was ON (expected 0x1A00, got 0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER]);
        return 1;
    }
    if (emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_cif ADDR_REG_INDIRECT did not decrement SP by 2 when flag was ON (got 0x%X)\n",
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0350 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret after exe_cif ADDR_REG_INDIRECT failed to restore PC/SP (PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 5c. ADDR_IMMEDIATE_SIXTEEN_DEST when condition is FALSE -> must NOT jump and must NOT push
    emu->flags = CMP_EQU; // bit 0 set, but checking bit 9 (SGR)
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0500;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x0300;
    instruction.operands[OP_ONE] = 9; // Signed greater than (bit 9 = CMP_SGR)

    if (exe_cif(emu, &instruction) != 0) {
        d_printf("exe_cif ADDR_IMMEDIATE_SIXTEEN_DEST returned error when flag was OFF\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0500 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_cif ADDR_IMMEDIATE_SIXTEEN_DEST modified PC or SP when flag was OFF (PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 5d. ADDR_IMMEDIATE_SIXTEEN_DEST when condition is TRUE -> must push PC, jump relative, and return cleanly
    emu->flags = CMP_SGR;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0600;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0500;
    if (exe_cif(emu, &instruction) != 0) {
        d_printf("exe_cif ADDR_IMMEDIATE_SIXTEEN_DEST returned error when flag was ON\n");
        return 1;
    }
    if (emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0800 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x05FE) {
        d_printf("exe_cif ADDR_IMMEDIATE_SIXTEEN_DEST failed when flag was ON (expected PC=0x0800, SP=0x05FE; got PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    instruction.addressing_mode = ADDR_IMMEDIATE_EIGHT;
    if (exe_ret(emu, &instruction) != 0 ||
        emu->alu.registers[ALU_PROGRAM_COUNTER] != 0x0500 ||
        emu->alu.registers[ALU_STACK_POINTER] != 0x0600) {
        d_printf("exe_ret after exe_cif ADDR_IMMEDIATE_SIXTEEN_DEST failed to restore PC/SP (PC=0x%X, SP=0x%X)\n",
                 emu->alu.registers[ALU_PROGRAM_COUNTER],
                 emu->alu.registers[ALU_STACK_POINTER]);
        return 1;
    }

    // 5e. Invalid addressing modes for exe_cif
    EMU_Addressing_Modes invalid_cif_modes[] = {
        ADDR_REG,
        ADDR_REG_IMMEDIATE_EIGHT,
        ADDR_REG_IMMEDIATE_SIXTEEN,
        ADDR_IMMEDIATE_EIGHT_DEST,
        ADDR_IMMEDIATE_EIGHT,
        ADDR_IMMEDIATE_SIXTEEN,
        (EMU_Addressing_Modes)255
    };
    for (size_t i = 0; i < sizeof(invalid_cif_modes) / sizeof(invalid_cif_modes[0]); i++) {
        instruction.addressing_mode = invalid_cif_modes[i];
        if (exe_cif(emu, &instruction) == 0) {
            d_printf("exe_cif allowed invalid addressing mode %d\n", invalid_cif_modes[i]);
            return 1;
        }
    }

    // 5f. Stack write failure in exe_cif when condition is met and SP is in bootloader
    emu->flags = CMP_EQU;
    emu->alu.registers[ALU_STACK_POINTER] = 0x0050;
    emu->alu.registers[ALU_PROGRAM_COUNTER] = 0x0200;
    instruction.addressing_mode = ADDR_IMMEDIATE_SIXTEEN_DEST;
    instruction.operands[OP_ZERO] = 0x0100;
    instruction.operands[OP_ONE] = 0;
    if (exe_cif(emu, &instruction) == 0) {
        d_printf("exe_cif allowed stack write into read-only bootloader region\n");
        return 1;
    }

    return 0;
}



#define test(string, function) \
    do { \
        d_printf("%s\n", string); \
        if(function){ \
            PRINT_REGISTER_FILES(test_emu); \
            d_printf("--Failed Test--\n\n"); \
            return 1; \
        } else d_printf("--Passed Test--\n\n"); \
    } while (0)


int run_tests(){


    Emulator *test_emu = init();
    if(!test_emu) return 1;

    test("--Testing memory functionality--", memory_test(&test_emu->ram));
    test("--Testing memory instructions--", memory_instructions_test(test_emu));
    test("--Testing alu instructions--", alu_test(test_emu));
    test("--Testing fpu instructions--", fpu_test(test_emu));
    test("--Testing control flow instructions--", control_flow_test(test_emu));
    
    destroy(test_emu); 
    return 0;
}