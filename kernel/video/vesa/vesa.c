#include "video/vesa/vesa.h"
#include "video/font.h"
#include "libc/assert.h"
#include "libc/string.h"

extern uint8_t _vbe_mode_struct[];
static struct vbe_mode_info_t *mode_info = (struct vbe_mode_info_t *) _vbe_mode_struct;

void _fill_screen(color_t color) {
    uint32_t *framebuffer = (uint32_t *) mode_info->framebuffer;
    for(size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for(size_t x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[y*SCREEN_WIDTH + x] = color;
        }
    }
}

void _print_char(unsigned char c, color_t color, size_t index, size_t scale) {
    uint32_t *framebuffer = (uint32_t *) mode_info->framebuffer;
    uint8_t *bitmap = font8x8[c];
    for(size_t y = 0; y < CHAR_HEIGHT; y++) {
        uint8_t row = bitmap[y];
        for(size_t x = 0; x < CHAR_WIDTH; x++) {
            if(((row >> x) & 1) == 0) {
                continue;
            }

            size_t offset = index + y*scale*SCREEN_WIDTH + x*scale;
            for(size_t sy = 0; sy < scale; sy++) {
                for(size_t sx = 0; sx < scale; sx++) {
                    framebuffer[offset + sy*SCREEN_WIDTH + sx] = color;
                }
            }
        }
    }
}