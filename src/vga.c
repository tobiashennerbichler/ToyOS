#include "vga.h"
#include "font.h"
#include "string.h"
#include "assert.h"

uint8_t *vga_buf = (uint8_t *) 0xA0000;
cursor_t cursor_pos = {0};

static size_t cur_to_index() {
    return cursor_pos.y * SCREEN_WIDTH + cursor_pos.x;
}

static void advance_cursor(size_t scale) {
    cursor_pos.x = (cursor_pos.x + 8*scale) % SCREEN_WIDTH;
    if(cursor_pos.x == 0) {
        cursor_pos.y = (cursor_pos.y + 8*scale) % SCREEN_HEIGHT;
    }
}

void init_vga() {
    fill_screen(BLACK);
}

void fill_screen(VGAColor color) {
    memset(vga_buf, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void write_char(char c, VGAColor color, size_t scale) {
    assert_msg(c >= 0, "Character must be positive");
    assert_msg((SCREEN_WIDTH % (8*scale)) == 0, "Scale must result in correct alignment");
    size_t index = cur_to_index();

    uint8_t *bitmap = font8x8[(unsigned int) c];
    for(size_t y = 0; y < 8; y++) {
        uint8_t row = bitmap[y];
        for(size_t x = 0; x < 8; x++) {
            if(((row >> x) & 1) == 0) {
                continue;
            }

            size_t offset = index + y*scale*SCREEN_WIDTH + x*scale;
            for(size_t sy = 0; sy < scale; sy++) {
                for(size_t sx = 0; sx < scale; sx++) {
                    vga_buf[offset + sy*SCREEN_WIDTH + sx] = color;
                }
            }
        }
    }
    advance_cursor(scale);
}

void write_string(const char *string, VGAColor color, size_t scale) {
    size_t len = strlen(string);
    for(size_t i = 0; i < len; i++) {
        write_char(string[i], color, scale);
    }
}

void write_int(int val, VGAColor color, size_t scale) {
    bool sign = val < 0;
    unsigned int uval = sign ? -val : val;

    char decimals[10] = {0};
    size_t num_decimals = 0;
    do {
        decimals[num_decimals] = uval % 10;
        uval /= 10;
        num_decimals++;
    } while(uval != 0);

    if(sign) write_char('-', color, scale);
    for(size_t i = 0; i < num_decimals; i++) {
        char c = decimals[num_decimals - i - 1] + '0';
        write_char(c, color, scale);
    }
}

void set_cursor(cursor_t pos) {
    // Bound to screen
    pos.x %= SCREEN_WIDTH;
    pos.y %= SCREEN_HEIGHT;

    // align to 8
    pos.x &= 7;
    pos.y &= 7;

    cursor_pos = pos;
}