#include "vga.h"
#include "string.h"
#include "assert.h"

uint8_t *pixel_buf = (uint8_t *) 0xA0000;
uint16_t *text_buf = (uint16_t *) 0xB8000;
cursor_t cursor_pos = {0};

static size_t cur_to_index() {
    return cursor_pos.y * TEXT_MODE_WIDTH + cursor_pos.x;
}

static void inc_cursor() {
    cursor_pos.x = (cursor_pos.x + 1) % TEXT_MODE_WIDTH;
    if(cursor_pos.x == 0) {
        cursor_pos.y = (cursor_pos.y + 1) % TEXT_MODE_HEIGHT;
    }
}

void init_vga() {
    for(size_t y = 0; y < TEXT_MODE_HEIGHT; y++) {
        for(size_t x = 0; x < TEXT_MODE_WIDTH; x++) {
            write_char(' ', BLACK);
        }
    }
}

void write_char(char c, VGAColor color) {
    size_t index = cur_to_index();
    text_buf[index] = c | (color << 8);
    inc_cursor();
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
    pos.x %= TEXT_MODE_WIDTH;
    pos.y %= TEXT_MODE_HEIGHT;

    cursor_pos = pos;
}
