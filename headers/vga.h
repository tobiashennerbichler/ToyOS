#ifndef __VGA_H__
#define __VGA_H__

#include <stddef.h>
#include <stdint.h>

#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25
#define PIXEL_MODE_WIDTH 320
#define PIXEL_MODE_HEIGHT 200

typedef struct cursor_t {
    size_t x;
    size_t y;
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
void write_char(char c, VGAColor color);
void write_string(const char *string, VGAColor color);
void write_int(int val, VGAColor color);
void set_cursor(cursor_t pos);

#endif