#include "video/vga/vga.h"
#include "video/font.h"
#include "libc/string.h"
#include "libc/assert.h"

uint8_t *framebuffer = (uint8_t *) 0xA0000;

void _fill_screen(color_t color) {
    memset(framebuffer, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void _print_char(unsigned char c, size_t index, size_t scale, color_t color) {
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