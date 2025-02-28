#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef VGA
#include "vga.h"
#else
#include "vesa.h"
#endif

#include "assert.h"

typedef struct cursor_t {
    uint16_t x;
    uint16_t y;
} cursor_t;

void init_graphics();
void fill_screen(color_t color);
void print_char(char c, color_t color);
void print_string(const char *string, color_t color);
void print_int(int val, color_t color);
void set_scale(size_t new_scale);
void set_cursor(cursor_t new_pos);
void reset_cursor();
void cursor_next_line();
void erase_prev_char();

#endif