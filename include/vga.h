#ifndef __VGA_H__
#define __VGA_H__

#include <stddef.h>
#include <stdint.h>

enum VGAColor {
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
};

typedef uint8_t color_t;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

void _fill_screen(color_t color);
void _print_char(char c, color_t color, size_t index, size_t scale);

#endif