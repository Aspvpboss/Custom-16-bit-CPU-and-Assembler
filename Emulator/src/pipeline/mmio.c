#include "pipeline/mmio.h"

void uart_out(u8 byte) {
    putchar(byte);
}
u8 uart_in() {
    return getchar();
}

int update_vram_buf(Emulator *emu) {
    u8 current_vram_bank = 0;
    SDL_SetAtomicU32(&emu->shared_data->signal_emu_active_vram_bank, emu->ram.vram_selector); 
    

}

int mmio(Emulator *emu){

    if(update_vram_buf(emu)) return 1;

    return 0;
}