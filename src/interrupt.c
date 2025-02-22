#include "interrupt.h"

__attribute__((aligned(16)))
struct idt_entry_t idt[NUM_ENTRIES] = { 0 };
const uint32_t KERN_CODE_SELECTOR = 8;

static void lidt(uint16_t limit, uint32_t base) {
    struct idtr_t idtr = {
        .limit = limit,
        .base = base,
        .reserved = 0
    };
    __asm__ volatile (
        "lidt %0\n"
        : : "m" (idtr)
    );
}

static void sti() {
    __asm__ volatile ("sti");
}

static void set_idt_entry(size_t index, uint32_t offset, enum gate_type type, enum ring privl) {
    if(index >= NUM_ENTRIES) {
        return;
    }

    uint8_t info = (1 << 7) | (privl << 5) | type;
    struct idt_entry_t entry = {
        .offset_low = offset & 0xFFFF,
        .selector = KERN_CODE_SELECTOR,
        .reserved = 0,
        .info = info,
        .offset_high = (offset >> 16) & 0xFFFF
    };

    idt[index] = entry;
}

void exception_handler(int interrupt_number) {
	uint16_t *buffer = (uint16_t *) 0xB8000;
	for(size_t y = 0; y < 25; y++) {
		for(size_t x = 0; x < 80; x++) {
			size_t index = y*80 + x;
			buffer[index] = ((char) interrupt_number + '0') | (1 << 8);
		}
	}

    __asm__ volatile (
        "cli\n"
        "hlt"
    );
}

extern void (*exception_table[32])(void);
void init_interrupts() {
    for(size_t i = 0; i < NUM_EXCEPTIONS; i++) {
        set_idt_entry(i, (uint32_t) exception_table[i], TRAP_GATE32, KERNEL);
    }

    lidt(NUM_ENTRIES * sizeof(struct idt_entry_t) - 1, (uint32_t) idt);
    //init_pic();
    //sti();
}
