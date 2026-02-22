#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA    0xCFC

uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
void scan_pci();

#endif