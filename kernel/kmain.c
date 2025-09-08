#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "graphics.h"
#include "keyboard.h"

#if !defined(__i386)
#error Incorrect target architecture
#endif

void print_entry() {
	char entry_text[] = "Welcome to ToyOS!";
	set_cursor((cursor_t) {
		.x = (SCREEN_WIDTH / 2) - (sizeof(entry_text)/2) * CHAR_WIDTH * get_scale(),
		.y = SCREEN_HEIGHT / 2
	});
	print_string(entry_text, LIGHT_GREEN);
	reset_cursor();
}

__attribute__((noreturn))
int kmain(void) {
	init_graphics(2, PINK, WHITE);
	init_interrupts();
	print_entry();

	//volatile int x = 42 / *((int *) &idt[NUM_EXCEPTIONS + 2]);
	while(1) {
		;
	}
}
