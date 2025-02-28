#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct keymap_t {
    unsigned char base_key;
    char shift_key;
    char alt_key;
} keymap_t;

typedef struct keyinfo_t {
    keymap_t keymap;
    void (*press_callback)(keymap_t);
    void (*release_callback)(keymap_t);
} keyinfo_t;

enum SpecialKeys {
    NONE = 0,
    ESCAPE = 0x80, BACKSPACE, TAB, ENTER,
    LEFT_CTRL, LEFT_SHIFT, RIGHT_SHIFT, KEYPAD_STAR,
    LEFT_ALT, CAPS_LOCK, F1, F2,
    F3, F4, F5, F6,
    F7, F8, F9, F10,
    NUM_LOCK, SCROLL_LOCK, KEYPAD_7, KEYPAD_8,
    KEYPAD_9, KEYPAD_MINUS, KEYPAD_4, KEYPAD_5,
    KEYPAD_6, KEYPAD_PLUS, KEYPAD_1, KEYPAD_2,
    KEYPAD_3, KEYPAD_0, KEYPAD_DOT, F11,
    F12
};

enum Flags {
    SHIFT = 1,
    CAPS = 2,
    ALT = 4
};

enum CommandId {
    ECHO = 0xEE,
    MANIP_SCAN_CODE = 0xF0,
    ENABLE_SCAN = 0xF4,
    DISABLE_SCAN
};

typedef struct Command {
    enum CommandId id;
    uint8_t data;
    uint8_t retries;
    uint8_t *ret;
} command_t;

enum Response {
    ACK = 0xFA,
    RESEND = 0xFE
};

typedef enum CommandState {
    IDLE,
    WAITING_FOR_RESP,
    WAITING_FOR_DATA
} CommandState;

#define KEYBOARD_PORT 0x60
#define TAB_SIZE 4
#define QUEUE_SIZE 0x10

int queue_command(command_t command);

#endif