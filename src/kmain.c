#include <stddef.h>
#include <stdint.h>
#include "interrupt.h"

#if !defined(__i386)
#error Incorrect target architecture
#endif

extern struct idt_entry_t idt[NUM_ENTRIES];
static void print_vga() {
	uint16_t *buffer = (uint16_t *) 0xB8000;
	for(size_t y = 0; y < 25; y++) {
		for(size_t x = 0; x < 80; x++) {
			size_t index = y*80 + x;
			buffer[index] = '@' | (1 << 8);
		}
	}
}

__attribute__((noreturn))
int kmain(void) {
	init_interrupts();
	print_vga();

	int y = *((int *) &idt[1]);
	volatile int x = 42 / y;
	while(1) {
		;
	}
}
