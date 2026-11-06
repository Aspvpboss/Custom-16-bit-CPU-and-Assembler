#include "pipeline/mmio.h"
#include "pipeline/execute/execute_memory.h"

#define TIME_SEC 0x7F00 // u16 R
#define TIME_MS 0x7F02 // u16 R
#define KEY_CURR 0x7F04 // u8 R
#define KEY_PREV 0x7F05 // u8 R
#define MOUSE_X 0x7F06 // u8 R
#define MOUSE_Y 0x7F07 // u8 R
#define MOUSE_BTN 0x7F08 // u8 R
#define VRAM_BUF_SIGNAL 0x7F09 // u8 W/R
#define DISP_CFG 0x7F0A // u8 W/R
#define DISP_VAL 0x7F0B // u16 W/R
#define UART_DATA 0x7F0D // u8 W/R
#define UART_STATUS 0x7F0E // u8 W/R

#define trunc_value(value, cond) cond ? value : value & 255 

void uart_out(u8 byte) {
    putchar(byte);
}
u8 uart_in() {
    return getchar();
}

int update_vram_buf(Emulator *emu) {

    if(!emu->mmio.vram_buf_swap_signal) return 0;
    u16 current_vram_bank = SDL_GetAtomicU32(&emu->shared_data->signal_emu_active_vram_bank);

    emu->ram.vram_selector = !current_vram_bank;
    SDL_SetAtomicU32(&emu->shared_data->signal_emu_active_vram_bank, emu->ram.vram_selector); 

    emu->mmio.vram_buf_swap_signal = false;

    return 0;
}

u16 get_current_uptime(Emulator *emu, bool get_millis) {

    struct timespec current_time;
    if(clock_gettime(CLOCK_MONOTONIC, &current_time))
        return 0;
    i64 sec_uptime = current_time.tv_sec - emu->mmio.base_time.tv_sec;
    i64 nsec_uptime = current_time.tv_nsec - emu->mmio.base_time.tv_nsec;
    if(nsec_uptime < 0){
        sec_uptime -= 1;
        nsec_uptime += 1000000000;
    } 
    
    if(get_millis)
        return (u16)(nsec_uptime / 1000000);
    else
        return (u16)sec_uptime;
}

int read_mmio(Emulator *emu, u16 address, u16 *read_value, bool sixteen_bit_read){
    switch(address){
        
        case TIME_SEC: {
            u16 secs = get_current_uptime(emu, false);
            *read_value = trunc_value(secs, sixteen_bit_read);
            break;
        }
        case TIME_MS: {
            u16 m_secs = get_current_uptime(emu, true);
            *read_value = trunc_value(m_secs, sixteen_bit_read);
            break;
        }
        case UART_DATA: {
            *read_value = (u16)uart_in(); 
            break;
        }

        default:
            return 1;
    }

    return 0;
}

int write_mmio(Emulator *emu, u16 address, u16 write_value, bool sixteen_bit_write) {

    switch(address) {
        
        case UART_DATA: {
            uart_out(write_value);
            break;
        }
        case VRAM_BUF_SIGNAL: {
            emu->mmio.vram_buf_swap_signal = true;
            if(update_vram_buf(emu)) return 1;
            break;
        }
        case DISP_CFG: {
            SDL_SetAtomicU32(&emu->shared_data->num_display_config, (u32)write_value); 
            break;
        }
        case DISP_VAL: {
            SDL_SetAtomicU32(&emu->shared_data->num_display_value, (u32)write_value); 
            break;
        }

        default:
            return 1;
    }

    return 0;
}


int mmio(Emulator *emu) {
    EMU_mmio *mmio = &emu->mmio; 

    if(update_vram_buf(emu)) return 1;


    return 0;
}