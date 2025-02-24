#ifndef __VGA_H__
#define __VGA_H__

#include <stddef.h>
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

typedef struct cursor_t {
    uint16_t x;
    uint16_t y;
} cursor_t;

typedef enum VGAColor {
    BLACK,
    DARK_BLUE,
    DARK_GREEN,
    TURQUOISE,
    DARK_RED,
    MAGENTA,
    ORANGE,
    LIGHT_GREY,
    DARK_GREY,
    MEDIUM_BLUE,
    LIGHT_GREEN,
    LIGHT_BLUE,
    LIGHT_RED,
    PINK,
    YELLOW,
    WHITE
} VGAColor;

void init_vga();
void fill_screen(VGAColor color);
void write_char(char c, VGAColor color, size_t scale);
void write_string(const char *string, VGAColor color, size_t scale);
void write_int(int val, VGAColor color, size_t scale);
void set_cursor(const cursor_t *new_pos);

#endif