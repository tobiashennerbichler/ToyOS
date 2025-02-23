#include "assert.h"
#include "vga.h"

void assert(bool cond) {
    if(cond) return;

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}

void assert_msg(bool cond, const char *err_msg) {
    if(cond) return;

    fill_screen(DARK_BLUE);
    set_cursor((cursor_t) {0});
    write_string(err_msg, DARK_RED, 1);

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}