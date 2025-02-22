#include "vga.h"
#include "font.h"
#include "string.h"
#include "assert.h"

uint8_t *vga_buf = (uint8_t *) 0xA0000;
cursor_t cursor_pos = {0};

static size_t cur_to_index() {
    return cursor_pos.y * SCREEN_WIDTH + cursor_pos.x;
}

static void advance_cursor() {
    cursor_pos.x = (cursor_pos.x + 8) % SCREEN_WIDTH;
    if(cursor_pos.x == 0) {
        cursor_pos.y = (cursor_pos.y + 8) % SCREEN_HEIGHT;
    }
}

void init_vga() {
    fill_screen(BLACK);
}

void fill_screen(VGAColor color) {
    memset(vga_buf, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void write_char(char c, VGAColor color) {
    ASSERT(c >= 0);
    size_t index = cur_to_index();

    uint8_t *bitmap = font8x8[(unsigned int) c];
    for(size_t y = 0; y < 8; y++) {
        uint8_t row = bitmap[y];
        for(size_t x = 0; x < 8; x++) {
            if((row >> x) & 1) {
                vga_buf[index + y*SCREEN_WIDTH + x] = color;
            }
        }
    }
    advance_cursor();
}

void write_string(const char *string, VGAColor color) {
    size_t len = strlen(string);
    for(size_t i = 0; i < len; i++) {
        write_char(string[i], color);
    }
}

void write_int(int val, VGAColor color) {
    bool sign = val < 0;
    unsigned int uval = sign ? -val : val;

    char decimals[10] = {0};
    size_t num_decimals = 0;
    do {
        ASSERT(num_decimals < 10);
        decimals[num_decimals] = uval % 10;
        uval /= 10;
        num_decimals++;
    } while(uval != 0);

    if(sign) write_char('-', color);
    for(size_t i = 0; i < num_decimals; i++) {
        char c = decimals[num_decimals - i - 1] + '0';
        write_char(c, color);
    }
}

void set_cursor(cursor_t pos) {
    pos.x %= SCREEN_WIDTH;
    pos.y %= SCREEN_HEIGHT;

    cursor_pos = pos;
}