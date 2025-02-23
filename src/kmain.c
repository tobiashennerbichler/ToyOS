#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "vga.h"

#if !defined(__i386)
#error Incorrect target architecture
#endif

extern struct idt_entry_t idt[256];
__attribute__((noreturn))
int kmain(void) {
	init_vga();
	init_interrupts();

	write_string("Welcome to ToyOS!", LIGHT_GREEN, 1);

	//volatile int x = 42 / *((int *) &idt[32]);
	while(1) {
		;
	}
}
