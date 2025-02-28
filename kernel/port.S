.intel_syntax noprefix

.global inb
.type inb, @function
# uint8_t inb(uint16_t port)
inb:
    mov dx, [esp+4]
    in al, dx
    ret

.global outb
.type outb, @function
# void outb(uint16_t port, uint8_t byte)
outb:
    mov dx, [esp+4]
    mov ax, [esp+8]
    out dx, al
    ret