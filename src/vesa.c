#include "vesa.h"
#include "assert.h"

extern uint8_t _vbe_mode_struct[];
static struct vbe_mode_info_t *mode_info = (struct vbe_mode_info_t *) _vbe_mode_struct;

void init_vesa() {
    uint32_t *framebuffer = (uint32_t *) mode_info->framebuffer;
    color_t blue = (color_t) {
        .blue = 0xff,
        .green = 0,
        .red = 0,
        .alpha = 0
    };

    for(size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for(size_t x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[y*SCREEN_WIDTH + x] = *((uint32_t *) &blue);
        }
    }
}