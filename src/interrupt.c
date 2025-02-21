#include "interrupt.h"

__attribute__((aligned(16)))
struct idt_entry_t idt[NUM_ENTRIES] = { 0 };
const uint32_t KERN_CODE_SELECTOR = 8;

static void _lidt(uint16_t limit, uint32_t base) {
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

static void _sti() {
    __asm__ volatile ("sti");
}

static void _set_entry(size_t index, uint32_t offset, enum gate_type type, enum ring privl) {
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

void init_interrupts() {
    ADD_EXCEPTION(0);
    _lidt(NUM_ENTRIES * sizeof(struct idt_entry_t) - 1, (uint32_t) &idt[0]);
    //init_pic();
    //_sti();
}
