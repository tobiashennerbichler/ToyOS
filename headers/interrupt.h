#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include <stddef.h>
#include <stdint.h>

#define NUM_ENTRIES 256
#define ADD_EXCEPTION(nr)   extern void wrap_exception_##nr(); _set_entry(nr, (uint32_t) wrap_exception_##nr, TRAP_GATE32, KERNEL)

enum gate_type { TASK_GATE = 5, INT_GATE16, TRAP_GATE16, INT_GATE32 = 14, TRAP_GATE32 };
enum ring { KERNEL = 0, USER = 3 };

struct __attribute__((packed)) idt_entry_t {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t info;
    uint16_t offset_high;
};

struct __attribute__((packed)) idtr_t {
    uint16_t limit;
    uint32_t base;
    uint16_t reserved;
};

void init_interrupts();

#endif