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
    // instruction.

    return 0;
}




#define test(string, function) \
    do { \
        d_printf("%s\n", string); \
        if(function){ \
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