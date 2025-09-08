#ifndef __PORT_H__
#define __PORT_H__

#include <stddef.h>
#include <stdint.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t byte);
static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif