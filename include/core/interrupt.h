#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_ENTRIES 256
#define NUM_EXCEPTIONS 32

#define MPIC_COMMAND    0x20
#define MPIC_DATA       (MPIC_COMMAND + 1)
#define SPIC_COMMAND    0xA0
#define SPIC_DATA       (SPIC_COMMAND + 1)
#define PIC_INIT        0x11
#define PIC_MODE_8086   0x01
#define PIC_EOI         0x20
#define PIC_READ_IRR    0x0a
#define PIC_READ_ISR    0x0b

#define TIMER_IRQ       0
#define KEYBOARD_IRQ    1
#define CASCADE         2
#define MOUSE_IRQ       12

#define IF_FLAG         0x200

typedef enum GateType { TASK_GATE = 5, INT_GATE16, TRAP_GATE16, INT_GATE32 = 14, TRAP_GATE32 } GateType;
typedef enum Ring { KERNEL = 0, USER = 3 } Ring;

typedef struct __attribute__((packed)) idt_entry_t {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t info;
    uint16_t offset_high;
} idt_entry_t;

typedef struct __attribute__((packed)) idtr_t {
    uint16_t limit;
    uint32_t base;
    uint16_t reserved;
} idtr_t;

void init_interrupts();
void cli();
void sti();
bool are_interrupts_enabled();

#endif