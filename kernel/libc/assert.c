#include "libc/assert.h"
#include "video/graphics.h"

//TODO: let them be disableable
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
    reset_cursor();
    print_string(err_msg, DARK_RED);

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}