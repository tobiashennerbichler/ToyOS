#ifndef __PORT_H__
#define __PORT_H__

#include <stddef.h>
#include <stdint.h>

uint8_t in8(uint16_t port);
uint32_t in32(uint16_t port);
void out8(uint16_t port, uint8_t byte);
void out32(uint16_t port, uint32_t dword);
static inline void io_wait(void) {
    out8(0x80, 0);
}

#endif