#include "interrupt.h"
#include "graphics.h"
#include "assert.h"
#include "port.h"

__attribute__((aligned(16)))
static idt_entry_t idt[NUM_ENTRIES] = { 0 };
static const uint32_t KERN_CODE_SELECTOR = 8;
static uint32_t spurious_counter = 0;

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
    reset_cursor();
    print_string("Exception: ", DARK_RED);
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
// Remap Master interrupts from 0-7 to 0x20-0x27
// Remap Slave interrupts from 0x70-0x77 to 0x28-0x2f
static void remap_pic() {
    uint8_t master_voff = 0x20;
    uint8_t slave_voff = 0x28;
    // Start Initialization
    outb(MPIC_COMMAND, PIC_INIT);
    io_wait();
    outb(SPIC_COMMAND, PIC_INIT);
    io_wait();
    
    // Send interrupt vector offsets
    outb(MPIC_DATA, master_voff);
    io_wait();
    outb(SPIC_DATA, slave_voff);
    io_wait();

    // Tell master about slave PIC at IRQ2
    outb(MPIC_DATA, 4);
    io_wait();
    outb(SPIC_DATA, 2);
    io_wait();

    // Switch to 8086 mode
    outb(MPIC_DATA, PIC_MODE_8086);
    io_wait();
    outb(SPIC_DATA, PIC_MODE_8086);
    io_wait();

    // Disable all IRQs by masking PICs
    clear_irqs();
}

static uint8_t pic_get_isr_reg(bool pic_master) {
    uint8_t port = pic_master ? MPIC_COMMAND : SPIC_COMMAND;
    outb(port, PIC_READ_ISR);
    return inb(port);
}

// Check if IRQ disappeared after PIC sent signaled interrupt
// In that case PIC sends lowest priority irq (7 for master, 15 for slave)
// Check ISR if interrupt should actually be serviced or if its spurious
bool is_spurious_int(uint8_t irq) {
    bool pic_master = (irq < 8);
    uint8_t bit = irq % 8;
    if(bit != 7) return false;

    uint8_t pic_isr = pic_get_isr_reg(pic_master);
    if(pic_isr & (1 << bit)) return false;

    spurious_counter++;
    if(!pic_master) {
        outb(MPIC_COMMAND, PIC_EOI);
    }
    return true;
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
    enable_irq(CASCADE); // Enable IRQ2 on the Master so the slave can send interrupts through it
    register_isr(KEYBOARD_IRQ);

    lidt(NUM_ENTRIES * sizeof(idt_entry_t) - 1, (uint32_t) idt);
    sti();
}

// IRQ 0-7: Master
// IRQ 8-15: Slave
void end_of_interrupt(uint8_t irq) {
    if(irq >= 8) {
        outb(SPIC_COMMAND, PIC_EOI);
    }

    outb(MPIC_COMMAND, PIC_EOI);
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