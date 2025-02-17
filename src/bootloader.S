org 0

INIT_CODE_SEGMENT	equ	0x7C0
DISK_CODE_SEGMENT	equ	0x7E0
RELOC_CODE_START	equ	0x200000
NUM_SECTORS		equ	10
SECTOR_SIZE		equ	512

cli
mov ax, INIT_CODE_SEGMENT
mov ss, ax
mov ds, ax
xor sp, sp
sti

cld

; Check INT 13 Extensions
mov ah, 0x41
mov bx, 0x55AA
int 0x13
jc error

; Load sectors from Disk
mov si, disk_packet
mov ah, 0x42
int 0x13

cli

; Enable A20 Line
; Use Fast Gate A20 instead of keyboard controller
; (https://aeb.win.tue.nl/linux/kbd/A20.html)
in al, 0x92
test al, 2
jnz a20_enabled
or al, 2
out 0x92, al

a20_enabled:
; Relocate the data from 0x7E00 to high memory at 0x200000
xor ax, ax
mov es, ax
mov ax, DISK_CODE_SEGMENT
mov ds, ax
xor si, si
mov edi, RELOC_CODE_START
mov cx, NUM_SECTORS*SECTOR_SIZE
reloc_loop:
	lodsb
	mov [es:edi], al
	inc edi
	dec cx
	jnz reloc_loop
end_loop:

mov si, done_msg
call print_string
; Jump to new code at 0x200000
jmp DWORD 0:RELOC_CODE_START

; si: Address of string
print_string:
	mov ax, 0x7C0
	mov ds, ax
	mov ah, 0xE
	xor bx, bx
	.print_loop:
		lodsb
		test al, al
		jz .end_loop
		int 0x10
		jmp .print_loop
	.end_loop:
		ret
	
; Print error code as ASCII char
error:
	mov al, ah
	add al, 0x30
	mov ah, 0xE
	xor bx, bx
	int 0x10
	hlt

disk_packet:
	db 16
	db 0
	dw NUM_SECTORS		; num sectors
	dw 0			; offset
	dw DISK_CODE_SEGMENT	; segment address
	dd 1			; lower 32-bit LDA address
	dd 0			; upper 16-bit LDA address

done_msg db "Booting into kernel...", 0

times 510 - ($ - $$) db 0
dw 0xAA55
