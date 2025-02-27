#ifndef __VESA_H__
#define __VESA_H__

#include <stddef.h>
#include <stdint.h>

typedef struct color_t {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} color_t;

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

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

void init_vesa();

#endif