#include "drivers/pci.h"
#include "core/port.h"
#include "video/graphics.h"

uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1ULL << 31) | (((uint32_t) bus) << 16) |
                       (((uint32_t) device) << 1) |
                       (((uint32_t) function) << 8) |
                       ((uint32_t) offset & 0xFC);
    
    out32(CONFIG_ADDRESS, address);
    return in32(CONFIG_DATA);
}

void pci_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (1ULL << 31) | (((uint32_t) bus) << 16) |
                       (((uint32_t) device) << 1) |
                       (((uint32_t) function) << 8) |
                       ((uint32_t) offset & 0xFC);

    out32(CONFIG_ADDRESS, address);
    out32(CONFIG_DATA, value);
}

//TODO: not finished yet
void scan_pci() {
    for(uint16_t bus = 0; bus < 256; bus++) {
        for(uint16_t device = 0; device < 256; device++) {
            uint32_t vendor_id = pci_read_dword(bus, device, 0, 0) & 0xffff;
            if(vendor_id != 0xffff) {
                print_int(bus, get_fg_color());
                print_char('/', get_fg_color());
                print_int(device, get_fg_color());
                print_char(':', get_fg_color());
                print_int(vendor_id, get_fg_color());
                print_string("  ", get_fg_color());
                uint32_t register2 = pci_read_dword(bus, device, 0, 8);
                uint8_t class_code = (register2 >> 24) & 0xff;
                uint8_t subclass = (register2 >> 16) & 0xff;
                uint8_t prog_if = (register2 >> 8) & 0xff;
                print_int(class_code, get_fg_color());
                print_char(',', get_fg_color());
                print_int(subclass, get_fg_color());
                print_char(',', get_fg_color());
                print_int(prog_if, get_fg_color());
                print_string("    ", get_fg_color());
            }
        }
    }
}