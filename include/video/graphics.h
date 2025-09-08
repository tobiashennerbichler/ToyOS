#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef VGA
#include "video/vga/vga.h"
#else
#include "video/vesa/vesa.h"
#endif

#include "libc/assert.h"

typedef struct cursor_t {
    uint16_t x;
    uint16_t y;
} cursor_t;

void init_graphics(uint8_t scale, color_t col_bg, color_t col_fg);
void fill_screen(color_t color);
void print_char(unsigned char c, color_t color);
void print_string(const char *string, color_t color);
void print_int(int32_t val, color_t color);
void set_bg_color(color_t col_bg);
void set_fg_color(color_t col_fg);
color_t get_bg_color();
color_t get_fg_color();
void set_scale(uint8_t new_scale);
uint8_t get_scale();
void set_cursor(cursor_t new_pos);
void reset_cursor();
void cursor_next_line();
void erase_prev_char();

#endif