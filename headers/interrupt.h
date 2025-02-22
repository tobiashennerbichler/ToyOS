#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stddef.h>
#include <stdint.h>

#define NUM_ENTRIES 256
#define NUM_EXCEPTIONS 32

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

#endif