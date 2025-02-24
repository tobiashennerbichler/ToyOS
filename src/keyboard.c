#include "keyboard.h"
#include "port.h"
#include "assert.h"
#include "vga.h"

// TODO: init function that sets scan code set
// TODO: allow for list of callbacks

uint8_t flags = 0;
static void press_flags(keymap_t keymap) {
    switch(keymap.base_key) {
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            flags |= SHIFT;
            break;
        case CAPS_LOCK:
            flags ^= CAPS;
            break;
        case LEFT_ALT:
            flags |= ALT;
            break;
    }
}

static void release_flags(keymap_t keymap) {
    switch(keymap.base_key) {
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            flags &= ~SHIFT;
            break;
        case LEFT_ALT:
            flags &= ~ALT;
            break;
    }
}

static void write(keymap_t keymap) {
    if(keymap.base_key < 0x80) {
        bool is_shift = flags & (SHIFT | CAPS);
        bool is_alt = flags & ALT;
        char c = is_shift ? keymap.shift_key :
                 is_alt   ? keymap.alt_key   :
                     (char) keymap.base_key;
        if(c == NONE)
            return;

        print_char(c, PINK);
    } else if(keymap.base_key == BACKSPACE) {
        erase_prev_char();
    } else if(keymap.base_key == TAB) {
        for(size_t i = 0; i < TAB_SIZE; i++) {
            print_char(' ', PINK);
        }
    } else if(keymap.base_key == ENTER) {
        cursor_next_line();
    }
}

// Corresponds to Scan Code Set 1 from: https://wiki.osdev.org/PS/2_Keyboard
// Remapped to fit an Austrian/German Keyboard Layout (ASCII chars)
// TODO: allow a list of callbacks to be provided
keyinfo_t scan_code_to_keyinfo[0x3B] = {
    {{NONE, NONE, NONE}, 0, 0},
    {{ESCAPE, NONE, NONE}, 0, 0},
    {{'1', '!', NONE}, write, 0},
    {{'2', '"', NONE}, write, 0},
    {{'3', NONE, NONE}, write, 0},
    {{'4', '$', NONE}, write, 0},
    {{'5', '%', NONE}, write, 0},
    {{'6', '&', NONE}, write, 0},
    {{'7', '/', '{'}, write, 0},
    {{'8', '(', '['}, write, 0},
    {{'9', ')', ']'}, write, 0},
    {{'0', '=', '}'}, write, 0},
    {{NONE, '?', '\\'}, write, 0},
    {{NONE, '`', NONE}, write, 0},
    {{BACKSPACE, NONE, NONE}, write, 0},
    {{TAB, NONE, NONE}, write, 0},
    {{'q', 'Q', NONE}, write, 0},
    {{'w', 'W', NONE}, write, 0},
    {{'e', 'E', NONE}, write, 0},
    {{'r', 'R', NONE}, write, 0},
    {{'t', 'T', NONE}, write, 0},
    {{'z', 'Z', NONE}, write, 0},
    {{'u', 'U', NONE}, write, 0},
    {{'i', 'I', NONE}, write, 0},
    {{'o', 'O', NONE}, write, 0},
    {{'p', 'P', NONE}, write, 0},
    {{NONE, NONE, NONE}, 0, 0}, // would be ü
    {{'+', '*', '~'}, write, 0},
    {{ENTER, NONE, NONE}, write, 0},
    {{LEFT_CTRL, NONE, NONE}, 0, 0},
    {{'a', 'A', NONE}, write, 0},
    {{'s', 'S', NONE}, write, 0},
    {{'d', 'D', NONE}, write, 0},
    {{'f', 'F', NONE}, write, 0},
    {{'g', 'G', NONE}, write, 0},
    {{'h', 'H', NONE}, write, 0},
    {{'j', 'J', NONE}, write, 0},
    {{'k', 'K', NONE}, write, 0},
    {{'l', 'L', NONE}, write, 0},
    {{NONE, NONE, NONE}, 0, 0}, // would be ö
    {{NONE, NONE, NONE}, 0, 0}, // would be ä
    {{'^', NONE, NONE}, write, 0},
    {{LEFT_SHIFT, NONE, NONE}, press_flags, release_flags},
    {{'#', '\'', NONE}, write, 0},
    {{'y', 'Y', NONE}, write, 0},
    {{'x', 'X', NONE}, write, 0},
    {{'c', 'C', NONE}, write, 0},
    {{'v', 'V', NONE}, write, 0},
    {{'b', 'B', NONE}, write, 0},
    {{'n', 'N', NONE}, write, 0},
    {{'m', 'M', NONE}, write, 0},
    {{',', ';', NONE}, write, 0},
    {{'.', ':', NONE}, write, 0},
    {{'-', '_', NONE}, write, 0},
    {{RIGHT_SHIFT, NONE, NONE}, press_flags, release_flags},
    {{KEYPAD_STAR, NONE, NONE}, 0, 0},
    {{LEFT_ALT, NONE, NONE}, press_flags, release_flags},
    {{' ', NONE, NONE}, write, 0},
    {{CAPS_LOCK, NONE, NONE}, press_flags, 0},
};

void keyboard_handler() {
    uint8_t scan_code = inb(KEYBOARD_PORT);

    bool released = scan_code >= 0x80;
    if(released)
        scan_code -= 0x80;
    // Extended keys not supported yet (> CAPS_LOCK)
    if(scan_code > 0x3A) {
        return;
    }

    keyinfo_t keyinfo = scan_code_to_keyinfo[scan_code];
    void (*callback)(keymap_t) = released ? keyinfo.release_callback : keyinfo.press_callback;
    if(callback) {
        callback(keyinfo.keymap);
    }
}