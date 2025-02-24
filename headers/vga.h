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

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

void init_vga();
void fill_screen(VGAColor color);
void print_char(char c, VGAColor color);
void print_string(const char *string, VGAColor color);
void print_int(int val, VGAColor color);
void set_scale(size_t new_scale);
void set_cursor(cursor_t new_pos);
void reset_cursor();
void cursor_next_line();
void erase_prev_char();

#endif