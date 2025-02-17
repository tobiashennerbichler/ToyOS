#include <stddef.h>
#include <stdint.h>

__attribute__((noreturn))
int main(void) {
	uint16_t *buffer = (uint16_t *) 0xB8000;
	for(size_t y = 0; y < 25; y++) {
		for(size_t x = 0; x < 80; x++) {
			size_t index = y*80 + x;
			buffer[index] = '@' | (7 << 8);
		}
	}

	while(1) {
		;
	}
}
