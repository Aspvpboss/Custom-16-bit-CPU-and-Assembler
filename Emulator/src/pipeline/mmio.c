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
    u8 current_vram_bank = 0;
    if(read_memory(emu, VRAM_BUF, &current_vram_bank, false)) return 1; 
    emu->ram.vram_selector = current_vram_bank;
    SDL_SetAtomicU32(&emu->shared_data->signal_emu_active_vram_bank, emu->ram.vram_selector); 

}

int mmio(Emulator *emu){

    if(update_vram_buf(emu)) return 1;

    return 0;
}