#include "vga.h"
#include "font.h"
#include "string.h"
#include "assert.h"

uint8_t *vga_buf = (uint8_t *) 0xA0000;
cursor_t pos = {0};
size_t scale = 1;

static size_t cur_to_index() {
    return pos.y * SCREEN_WIDTH + pos.x;
}

static void advance_cursor() {
    pos.x += CHAR_WIDTH * scale;
    if(pos.x >= SCREEN_WIDTH) {
        pos.x %= SCREEN_WIDTH;
        pos.y += CHAR_HEIGHT * scale;
        pos.y %= SCREEN_HEIGHT;
    }
}

static void redvance_cursor() {
    if(pos.x == 0) {
        if(pos.y == 0) {
            return;
        }

        pos.y -= CHAR_HEIGHT * scale;
        pos.x = SCREEN_WIDTH - CHAR_WIDTH * scale;
    } else {
        pos.x -= CHAR_WIDTH * scale;
    }
}

void init_vga() {
    fill_screen(BLACK);
}

void fill_screen(VGAColor color) {
    memset(vga_buf, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void print_char(char c, VGAColor color) {
    assert_msg(c >= 0, "Character must be positive");
    size_t index = cur_to_index();

    uint8_t *bitmap = font8x8[(unsigned int) c];
    for(size_t y = 0; y < CHAR_HEIGHT; y++) {
        uint8_t row = bitmap[y];
        for(size_t x = 0; x < CHAR_WIDTH; x++) {
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
    advance_cursor();
}

void print_string(const char *string, VGAColor color) {
    size_t len = strlen(string);
    for(size_t i = 0; i < len; i++) {
        print_char(string[i], color);
    }
}

void print_int(int val, VGAColor color) {
    bool sign = val < 0;
    unsigned int uval = sign ? -val : val;

    char decimals[10] = {0};
    size_t num_decimals = 0;
    do {
        decimals[num_decimals] = uval % 10;
        uval /= 10;
        num_decimals++;
    } while(uval != 0);

    if(sign) print_char('-', color);
    for(size_t i = 0; i < num_decimals; i++) {
        char c = decimals[num_decimals - i - 1] + '0';
        print_char(c, color);
    }
}

void set_scale(size_t new_scale) {
    assert_msg((SCREEN_WIDTH % (CHAR_WIDTH * scale)) == 0, "Scale must result in correct alignment");
    
    fill_screen(BLACK);
    reset_cursor();
    scale = new_scale;
}

// TODO: should set based on char size
void set_cursor(cursor_t new_pos) {
    // Bound to screen
    new_pos.x %= SCREEN_WIDTH;
    new_pos.y %= SCREEN_HEIGHT;

    // align to CHAR_HEIGHT/WIDTH * scale
    new_pos.x -= (new_pos.x % (CHAR_WIDTH * scale));
    new_pos.y -= (new_pos.y % (CHAR_HEIGHT * scale));
    pos = new_pos;
}

void reset_cursor() {
    set_cursor((cursor_t) {0});
}

void cursor_next_line() {
    pos.x = 0;
    pos.y += CHAR_HEIGHT * scale;
    if(pos.y >= SCREEN_HEIGHT)
        pos.y = 0;
}

void erase_prev_char() {
    redvance_cursor();
    print_char(0, BLACK);
    redvance_cursor();
}