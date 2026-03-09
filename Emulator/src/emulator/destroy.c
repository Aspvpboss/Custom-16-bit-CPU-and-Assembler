#include "destroy.h"


void destroy_emu(Emulator *emu){

    SDLite_DestroyDisplay(emu->SDLite_io.display);
    SDLite_DestroyInput(emu->SDLite_io.input);
    SDLite_DestroyTime(emu->SDLite_io.time);
    t_free(emu);
}


void destroy(Emulator *emu){

    destroy_emu(emu);

}