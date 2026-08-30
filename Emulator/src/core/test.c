#include "core/init.h"
#include "core/iterate.h"
#include "core/destroy.h"
#include "core/test.h"
#include "pipeline/execute/execute_math.h"
#include "pipeline/execute/execute_memory.h"
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




int memory_test(EMU_Ram *ram){

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

int memory_instructions_test(Emulator *emu){
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




#define test(string, function) \
    do { \
        d_printf("%s\n", string); \
        if(function){ \
            PRINT_REGISTER_FILES(test_emu); \
            d_printf("--Failed Test--\n\n"); \
            failed_test = true; \
        } else d_printf("--Passed Test--\n\n"); \
    } while (0)


int run_tests(){

    bool failed_test = false;

    Emulator *test_emu = init();
    if(!test_emu) return 1;

    test("--Testing memory functionality--", memory_test(&test_emu->ram));
    test("--Testing memory instructions--", memory_instructions_test(test_emu));

    destroy(test_emu); 
    return failed_test;
}