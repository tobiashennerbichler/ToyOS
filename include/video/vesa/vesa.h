#ifndef __VESA_H__
#define __VESA_H__

#include <stddef.h>
#include <stdint.h>

typedef uint32_t color_t;

typedef struct cursor_t {
	uint16_t x;
	uint16_t y;
} cursor_t;

// Sourced from: https://wiki.osdev.org/User:Omarrx024/VESA_Tutorial
struct __attribute__((packed)) vbe_mode_info_t {
    uint16_t attributes;
    uint8_t unused1[14];
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;
	uint16_t height;
    uint8_t unused2[3];
	uint8_t bpp;
    uint8_t unused3[14];
	uint32_t framebuffer;
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
};

enum VesaColor {
    BLACK = 0,
    DARK_BLUE = 0x80,
    DARK_GREEN = 0x38761d,
    TURQUOISE = 0x7fffd4,
    DARK_RED = 0x800000,
    MAGENTA = 0x800080,
    ORANGE = 0xffa500,
    LIGHT_GREY = 0xc0c0c0,
    DARK_GREY = 0x666666,
    BLUE = 0xff,
    LIGHT_GREEN = 0xff00,
    LIGHT_BLUE = 0xc6e2ff,
    LIGHT_RED = 0xff0000,
    PINK = 0xe189a5,
    YELLOW = 0xffff00,
    WHITE = 0xffffff
};

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define CHAR_HEIGHT 8
#define CHAR_WIDTH 8

void _fill_screen(color_t color);
void _print_char(unsigned char c, color_t color, size_t index, size_t scale);

#endif