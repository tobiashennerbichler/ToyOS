#include "keyboard.h"
#include "port.h"
#include "assert.h"
#include "graphics.h"
#include "interrupt.h"
#include "string.h"

// TODO: init function that sets scan code set
// TODO: allow for list of callbacks
static command_t command_queue[QUEUE_SIZE] = {0};
static size_t num_commands = 0;
static CommandState state = IDLE;
static uint8_t flags = 0;

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

static void display_key(keymap_t keymap) {
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
static keyinfo_t scan_code_to_keyinfo[0x3B] = {
    {{NONE, NONE, NONE}, 0, 0},
    {{ESCAPE, NONE, NONE}, 0, 0},
    {{'1', '!', NONE}, display_key, 0},
    {{'2', '"', NONE}, display_key, 0},
    {{'3', NONE, NONE}, display_key, 0},
    {{'4', '$', NONE}, display_key, 0},
    {{'5', '%', NONE}, display_key, 0},
    {{'6', '&', NONE}, display_key, 0},
    {{'7', '/', '{'}, display_key, 0},
    {{'8', '(', '['}, display_key, 0},
    {{'9', ')', ']'}, display_key, 0},
    {{'0', '=', '}'}, display_key, 0},
    {{NONE, '?', '\\'}, display_key, 0},
    {{NONE, '`', NONE}, display_key, 0},
    {{BACKSPACE, NONE, NONE}, display_key, 0},
    {{TAB, NONE, NONE}, display_key, 0},
    {{'q', 'Q', NONE}, display_key, 0},
    {{'w', 'W', NONE}, display_key, 0},
    {{'e', 'E', NONE}, display_key, 0},
    {{'r', 'R', NONE}, display_key, 0},
    {{'t', 'T', NONE}, display_key, 0},
    {{'z', 'Z', NONE}, display_key, 0},
    {{'u', 'U', NONE}, display_key, 0},
    {{'i', 'I', NONE}, display_key, 0},
    {{'o', 'O', NONE}, display_key, 0},
    {{'p', 'P', NONE}, display_key, 0},
    {{NONE, NONE, NONE}, 0, 0}, // would be ü
    {{'+', '*', '~'}, display_key, 0},
    {{ENTER, NONE, NONE}, display_key, 0},
    {{LEFT_CTRL, NONE, NONE}, 0, 0},
    {{'a', 'A', NONE}, display_key, 0},
    {{'s', 'S', NONE}, display_key, 0},
    {{'d', 'D', NONE}, display_key, 0},
    {{'f', 'F', NONE}, display_key, 0},
    {{'g', 'G', NONE}, display_key, 0},
    {{'h', 'H', NONE}, display_key, 0},
    {{'j', 'J', NONE}, display_key, 0},
    {{'k', 'K', NONE}, display_key, 0},
    {{'l', 'L', NONE}, display_key, 0},
    {{NONE, NONE, NONE}, 0, 0}, // would be ö
    {{NONE, NONE, NONE}, 0, 0}, // would be ä
    {{'^', NONE, NONE}, display_key, 0},
    {{LEFT_SHIFT, NONE, NONE}, press_flags, release_flags},
    {{'#', '\'', NONE}, display_key, 0},
    {{'y', 'Y', NONE}, display_key, 0},
    {{'x', 'X', NONE}, display_key, 0},
    {{'c', 'C', NONE}, display_key, 0},
    {{'v', 'V', NONE}, display_key, 0},
    {{'b', 'B', NONE}, display_key, 0},
    {{'n', 'N', NONE}, display_key, 0},
    {{'m', 'M', NONE}, display_key, 0},
    {{',', ';', NONE}, display_key, 0},
    {{'.', ':', NONE}, display_key, 0},
    {{'-', '_', NONE}, display_key, 0},
    {{RIGHT_SHIFT, NONE, NONE}, press_flags, release_flags},
    {{KEYPAD_STAR, NONE, NONE}, 0, 0},
    {{LEFT_ALT, NONE, NONE}, press_flags, release_flags},
    {{' ', NONE, NONE}, display_key, 0},
    {{CAPS_LOCK, NONE, NONE}, press_flags, 0},
};

static void command_remove_head() {
    for(size_t i = 0; i < (num_commands - 1); i++) {
        command_queue[i] = command_queue[i + 1];
    }
    memset((uint8_t *) &command_queue[num_commands - 1], 0, sizeof(command_t));
    num_commands--;
}

static void send_command(command_t command) {
    outb(KEYBOARD_PORT, (uint8_t) command.id);
    io_wait();
    outb(KEYBOARD_PORT, command.data);
}

static bool send_next_command() {
    if(num_commands == 0) {
        return false;
    }

    send_command(command_queue[0]);
    return true;
}

static void process_keypress(uint8_t scan_code) {
    bool released = scan_code >= 0x80;
    if(released)
        scan_code -= 0x80;

    // Extended keys not supported yet (> CAPS_LOCK)
    if(scan_code > 0x3A) {
        return;
    }

    keyinfo_t keyinfo = scan_code_to_keyinfo[scan_code];
    void (*callback)(keymap_t) = released ? keyinfo.release_callback
                                          : keyinfo.press_callback;
    if(callback) {
        callback(keyinfo.keymap);
    }
}

static void process_response(uint8_t scan_code) {
    assert_msg(num_commands != 0, "No command in queue");
    command_t *head = &command_queue[0];

    if(scan_code == ACK) {
        // ACK on SetScanCode is followed by scan code setting
        if(head->id == MANIP_SCAN_CODE) {
            state = WAITING_FOR_DATA;
            return;
        }

        if(head->ret)
            *head->ret = 0;
        command_remove_head();
        state = send_next_command() ? WAITING_FOR_RESP : IDLE;
    } else if(scan_code == RESEND) {
        // RESEND on GetScanCode is followed by scan code setting
        if(head->id == MANIP_SCAN_CODE) {
            state = WAITING_FOR_DATA;
            return;
        }

        if(head->retries == 0) {
            if(head->ret)
                *head->ret = -1;
            command_remove_head();
        } else {
            head->retries--;
        }
        state = send_next_command() ? WAITING_FOR_RESP : IDLE;
    } else if(scan_code == ECHO) {
        assert_msg(head->id == ECHO, "ECHO received on non-ECHO command");
        if(head->ret)
            *head->ret = ECHO;
        command_remove_head();
        state = send_next_command() ? WAITING_FOR_RESP : IDLE;
    // Might be standard keypress
    } else {
        process_keypress(scan_code);
    }
}

static void process_data(uint8_t scan_code) {
    assert_msg(num_commands != 0, "No command in queue");
    command_t *head = &command_queue[0];
    assert_msg(head->id == MANIP_SCAN_CODE, "Waiting for data without correct command");

    if(head->ret)
        *head->ret = scan_code;
    command_remove_head();
    state = send_next_command() ? WAITING_FOR_RESP : IDLE;
}

void keyboard_handler() {
    uint8_t scan_code = inb(KEYBOARD_PORT);

    switch(state) {
        case IDLE:
            process_keypress(scan_code);
            break;
        case WAITING_FOR_RESP:
            process_response(scan_code);
            break;
        case WAITING_FOR_DATA:
            process_data(scan_code);
            break;
    }
}

int queue_command(command_t command) {
    bool enabled = are_interrupts_enabled();
    cli();

    if(num_commands == QUEUE_SIZE) {
        return -1;
    }
    command_queue[num_commands++] = command;

    if(state == IDLE) {
        send_command(command);
        state = WAITING_FOR_RESP;
    }

    if(enabled)
        sti();

    return 0;
}