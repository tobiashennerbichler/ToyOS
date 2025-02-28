#include "interrupt.h"
#include "graphics.h"
#include "assert.h"
#include "port.h"

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

const char *exception_names[32] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved: 15",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved: 22",
    "Reserved: 23",
    "Reserved: 24",
    "Reserved: 25",
    "Reserved: 26",
    "Reserved: 27",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved: 31"
};
void exception_handler(int interrupt_number) {
    fill_screen(DARK_BLUE);
    char text[] = "Exception: ";
    reset_cursor();
    print_string(text, DARK_RED);
    print_string(exception_names[interrupt_number], DARK_RED);

    assert(false);
}

static void clear_irqs() {
    outb(MPIC_DATA, 0xff);
    outb(SPIC_DATA, 0xff);
}

static void enable_irq(uint8_t irq_number) {
    uint8_t port = (irq_number >= 8) ? SPIC_DATA : MPIC_DATA;
    uint8_t bit = irq_number % 8;

    uint8_t status = inb(port);
    outb(port, status & ~(1 << bit));
}

// https://wiki.osdev.org/8259_PIC#Programming_with_the_8259_PIC
static void remap_pic() {
    uint8_t master_voff = 0x20;
    uint8_t slave_voff = 0x28;
    // Init requires: 1. Vector offset 2. Wire info 3. Env info
    outb(MPIC_COMMAND, INIT_PIC);
    io_wait();
    outb(SPIC_COMMAND, INIT_PIC);
    io_wait();
    
    outb(MPIC_DATA, master_voff);
    io_wait();
    outb(SPIC_DATA, slave_voff);
    io_wait();

    outb(MPIC_DATA, 4);
    io_wait();
    outb(SPIC_DATA, 2);
    io_wait();

    outb(MPIC_DATA, MODE_8086);
    io_wait();
    outb(SPIC_DATA, MODE_8086);
    io_wait();

    // Disable all IRQs by masking PICs
    clear_irqs();
}

extern void (*isr_table[16])(void);
static void register_isr(uint8_t irq_number) {
    enable_irq(irq_number);
    set_idt_entry(NUM_EXCEPTIONS + irq_number, (uint32_t) isr_table[irq_number], INT_GATE32, KERNEL);
}

extern void (*exception_table[32])(void);
void init_interrupts() {
    for(size_t i = 0; i < NUM_EXCEPTIONS; i++) {
        set_idt_entry(i, (uint32_t) exception_table[i], TRAP_GATE32, KERNEL);
    }
    
    remap_pic();
    enable_irq(CASCADE);
    register_isr(KEYBOARD_IRQ);

    lidt(NUM_ENTRIES * sizeof(idt_entry_t) - 1, (uint32_t) idt);
    sti();
}

// IRQ 0-7: Master
// IRQ 8-15: Slave
void end_of_interrupt(uint8_t irq) {
    if(irq >= 8) {
        outb(SPIC_COMMAND, EOI);
    }

    outb(MPIC_COMMAND, EOI);
}

inline void cli() {
    __asm__ volatile ("cli");
}

inline void sti() {
    __asm__ volatile ("sti");
}

inline bool are_interrupts_enabled() {
    uint32_t status;
    __asm__ volatile (
        "pushf\n"
        "pop %0\n"
        : "=g"(status)
    );
    return (status & IF_FLAG);
}