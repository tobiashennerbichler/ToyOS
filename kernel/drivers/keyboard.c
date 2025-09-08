#include "drivers/keyboard.h"
#include "drivers/keyboard_mapping.h"
#include "core/port.h"
#include "core/interrupt.h"
#include "video/graphics.h"
#include "libc/assert.h"
#include "libc/string.h"

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

        print_char(c, get_fg_color());
    } else if(keymap.base_key == BACKSPACE) {
        erase_prev_char();
    } else if(keymap.base_key == TAB) {
        for(size_t i = 0; i < TAB_SIZE; i++) {
            print_char(' ', get_fg_color());
        }
    } else if(keymap.base_key == ENTER) {
        cursor_next_line();
    }
}

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