.intel_syntax noprefix

.global in8
.type in8, @function
# uint8_t in8(uint16_t port)
in8:
    mov dx, [esp+4]
    in al, dx
    ret

.global in32
.type in32, @function
# uint32_t int32(uint16_t port)
in32:
    mov dx, [esp+4]
    in eax, dx
    ret

.global out8
.type out8, @function
# void out8(uint16_t port, uint8_t byte)
out8:
    mov dx, [esp+4]
    mov ax, [esp+8]
    out dx, al
    ret

.global out32
.type out32, @function
# void out32(uint16_t port, uint32_t dword)
out32:
    mov dx, [esp+4]
    mov eax, [esp+8]
    out dx, eax
    ret