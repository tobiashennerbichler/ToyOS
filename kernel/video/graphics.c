#include "video/graphics.h"
#include "libc/string.h"

static cursor_t _pos = {0};
static uint8_t _scale = 2;
static color_t _col_bg = BLACK;
static color_t _col_fg = WHITE;

static size_t cur_to_index() {
    return _pos.y * SCREEN_WIDTH + _pos.x;
}

static void advance_cursor() {
    _pos.x += CHAR_WIDTH * _scale;
    uint16_t inc_y = _pos.x / SCREEN_WIDTH;
    _pos.x %= SCREEN_WIDTH;
    _pos.y += (inc_y * CHAR_HEIGHT * _scale) % SCREEN_HEIGHT;
}

static void redvance_cursor() {
    if(_pos.x == 0) {
        if(_pos.y == 0) {
            return;
        }

        _pos.y -= CHAR_HEIGHT * _scale;
        _pos.x = SCREEN_WIDTH - CHAR_WIDTH * _scale;
    } else {
        _pos.x -= CHAR_WIDTH * _scale;
    }
}

inline void set_bg_color(color_t col_bg) {
    _col_bg = col_bg;
}

inline void set_fg_color(color_t col_fg) {
    _col_fg = col_fg;
}

inline color_t get_bg_color() {
    return _col_bg;
}

inline color_t get_fg_color() {
    return _col_fg;
}

void fill_screen(color_t color) {
    _fill_screen(color);
}

void init_graphics(uint8_t scale, color_t col_bg, color_t col_fg) {
    set_scale(scale);
    _col_bg = col_bg;
    _col_fg = col_fg;

    fill_screen(_col_bg);
}

void print_char(unsigned char c, color_t color) {
    size_t index = cur_to_index();
    _print_char(c, color, index, _scale);
    advance_cursor();
}

void print_string(const char *string, color_t color) {
    size_t len = strlen(string);
    for(size_t i = 0; i < len; i++) {
        print_char(string[i], color);
    }
}

void print_int(int32_t val, color_t color) {
    bool sign = val < 0;
    uint32_t uval = sign ? -val : val;

    char decimals[10] = {0}; // log10(2^31) = 9,33
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

void set_scale(uint8_t new_scale) {
    assert_msg((SCREEN_WIDTH % (CHAR_WIDTH * _scale)) == 0, "Scale must result in correct alignment");
    
    _scale = new_scale;
    // Redraw entire screen
    fill_screen(_col_bg);
    reset_cursor();
}

inline uint8_t get_scale() {
    return _scale;
}

inline cursor_t get_cursor() {
    return _pos;
}

void set_cursor(cursor_t new_pos) {
    // Bound to screen
    new_pos.x %= SCREEN_WIDTH;
    new_pos.y %= SCREEN_HEIGHT;

    // align to CHAR_HEIGHT/WIDTH * scale
    new_pos.x -= (new_pos.x % (CHAR_WIDTH * _scale));
    new_pos.y -= (new_pos.y % (CHAR_HEIGHT * _scale));
    _pos = new_pos;
}

void reset_cursor() {
    set_cursor((cursor_t) {0});
}

void cursor_next_line() {
    _pos.x = 0;
    _pos.y += (CHAR_HEIGHT * _scale) % SCREEN_HEIGHT;
}

void erase_prev_char() {
    redvance_cursor();
    print_char(0, _col_bg);
    redvance_cursor();
}