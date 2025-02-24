#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "vga.h"

#if !defined(__i386)
#error Incorrect target architecture
#endif

void print_entry() {
	char entry_text[] = "Welcome to ToyOS!";
	set_scale(1);
	set_cursor((cursor_t) {
		.x = (SCREEN_WIDTH / 2) - sizeof(entry_text),
		.y = SCREEN_HEIGHT / 2
	});
	print_string(entry_text, LIGHT_GREEN);
	reset_cursor();
}

extern struct idt_entry_t idt[256];
__attribute__((noreturn))
int kmain(void) {
	init_vga();
	init_interrupts();
	print_entry();

	//volatile int x = 42 / *((int *) &idt[32]);
	while(1) {
		;
	}
}
