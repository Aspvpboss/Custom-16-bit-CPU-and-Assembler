#ifndef DESTROY_H_
#define DESTROY_H_

#include "hardware/emulator.h"
#include "core/render_update.h"

void destroy(Emulator *emu);
void destroy_render_data(RenderData *render_data);
void destroy_shared_data(SharedData *shared_data);

#endif