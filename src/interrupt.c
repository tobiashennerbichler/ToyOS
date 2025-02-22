#include "interrupt.h"
#include "vga.h"
#include "assert.h"

__attribute__((aligned(16)))
idt_entry_t idt[NUM_ENTRIES] = { 0 };
const uint32_t KERN_CODE_SELECTOR = 8;

static void lidt(uint16_t limit, uint32_t base) {
    idtr_t idtr = {
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

static void set_idt_entry(size_t index, uint32_t offset, GateType type, Ring privl) {
    if(index >= NUM_ENTRIES) {
        return;
    }

    uint8_t info = (1 << 7) | (privl << 5) | type;
    idt_entry_t entry = {
        .offset_low = offset & 0xFFFF,
        .selector = KERN_CODE_SELECTOR,
        .reserved = 0,
        .info = info,
        .offset_high = (offset >> 16) & 0xFFFF
    };

    idt[index] = entry;
}

void exception_handler(int interrupt_number) {
    char text[] = "Error: ";
    write_string(text, MAGENTA);
    write_int(interrupt_number, MAGENTA);

    ASSERT(false);
}

extern void (*exception_table[32])(void);
void init_interrupts() {
    for(size_t i = 0; i < NUM_EXCEPTIONS; i++) {
        set_idt_entry(i, (uint32_t) exception_table[i], TRAP_GATE32, KERNEL);
    }

    lidt(NUM_ENTRIES * sizeof(idt_entry_t) - 1, (uint32_t) idt);
    //init_pic();
    //sti();
}
