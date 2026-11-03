#include "pipeline/mmio.h"
#include "pipeline/execute/execute_memory.h"

const u16 TIME_SEC = 0x7F00; // u16 R
const u16 TIME_MS = 0x7F02; // u16 R
const u16 KEY_CURR = 0x7F04; // u8 R
const u16 KEY_PREV = 0x7F05; // u8 R
const u16 MOUSE_X = 0x7F06; // u8 R
const u16 MOUSE_Y = 0x7F07; // u8 R
const u16 MOUSE_BTN = 0x7F08; // u8 R
const u16 VRAM_BUF = 0x7F09; // u8 W/R
const u16 DISP_CFG = 0x7F0A; // u8 W/R
const u16 DISP_VAL = 0x7F0B; // u16 W/R
const u16 UART_DATA = 0x7F0D; // u8 W/R
const u16 UART_STATUS = 0x7F0E; // u8 W/R



void uart_out(u8 byte) {
    putchar(byte);
}
u8 uart_in() {
    return getchar();
}

int update_vram_buf(Emulator *emu) {
    static u16 prev_vram_bank = 0;
    u16 current_vram_bank = 0;
    if(read_memory(emu, VRAM_BUF, &current_vram_bank, false)){
        return 1;
    }
    if(prev_vram_bank == current_vram_bank) return 0;
        
    emu->ram.vram_selector = current_vram_bank;
    SDL_SetAtomicU32(&emu->shared_data->signal_emu_active_vram_bank, emu->ram.vram_selector); 

    return 0;
}

int update_time(Emulator *emu) {

    static struct timespec base_time = {0};

    if(base_time.tv_sec == 0 && base_time.tv_nsec == 0){
        clock_gettime(CLOCK_MONOTONIC, &base_time);
    } 
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    i64 sec_uptime = current_time.tv_sec - base_time.tv_sec;
    i64 nsec_uptime = current_time.tv_nsec - base_time.tv_nsec;
    if(nsec_uptime < 0){
        sec_uptime -= 1;
        nsec_uptime += 1000000000;
    } 

    i64 milli_uptime = (i64)(nsec_uptime / 1000000);
    if(write_memory(emu, TIME_SEC, (u16)sec_uptime, true))
        return 1;
    if(write_memory(emu, TIME_MS, (u16)milli_uptime, true))
        return 1;
    
    return 0;
}

int read_mmio(Emulator *emu, u16 address, u16 *read_value){
    if(update_vram_buf(emu)) return 1;
    if(update_time(emu)) return 1;

    return 0;
}

int write_mmio(Emulator *emu, u16 address, u16 write_value) {


    return 0;
}


int mmio(Emulator *emu) {


    return 0;
}