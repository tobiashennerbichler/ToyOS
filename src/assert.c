#include "assert.h"
#include "vga.h"

// TODO: seems to not halt
void assert(bool cond) {
    if(cond) return;

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}

void assert_msg(bool cond, const char *err_msg) {
    if(cond) return;

    cursor_t pos = {0};
    set_cursor(pos);
    write_string(err_msg, DARK_RED, 1);

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}