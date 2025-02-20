BOOT_CODE_ADDRESS	equ	0x7C00
DISK_CODE_ADDRESS	equ	0x7E00
RELOC_CODE_START	equ	0x200000
NUM_SECTORS			equ	1
SECTOR_SIZE			equ	512

[BITS 16]
[ORG 0x7C00]
jmp init
nop

; si: Address of string
print_string:
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

init:
	cli
	xor ax, ax
	mov ss, ax
	mov ds, ax
	mov sp, BOOT_CODE_ADDRESS
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
	jc error

prepare_switch:
	cli

	; Enable A20 Line
	; Use Fast Gate A20 instead of keyboard controller
	; (https://aeb.win.tue.nl/linux/kbd/A20.html)
	in al, 0x92
	test al, 2
	jnz .load_gdt
	or al, 2
	out 0x92, al

	;.reloc_code:
	; Relocate the data from 0x7E00 to high memory at 0x200000
	;xor ax, ax
	;mov es, ax
	;mov ax, DISK_CODE_SEGMENT
	;mov ds, ax
	;xor si, si
	;mov edi, RELOC_CODE_START
	;mov cx, NUM_SECTORS*SECTOR_SIZE
	; rep movsb

	; Load GDT
	.load_gdt:
	lgdt [gdt]

switch:
	;mov si, done_msg
	;call print_string

	; Switch to protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; Far Jump to load cs as kernel code segment selector
	jmp 0x08:protected

[BITS 32]
protected:
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	jmp 0x7E00

disk_packet:
	db 16
	db 0
	dw NUM_SECTORS			; num sectors
	dw DISK_CODE_ADDRESS	; offset
	dw 0					; segment address
	dd 1					; lower 32-bit LDA address
	dd 0					; upper 16-bit LDA address

gdt:
	; Null Entry
	dw 40
	dd gdt
	dw 0
	; Kernel Code
	dw 0xFFFF		; lower 16-bit of limit
	dw 0			; lower 16-bit of base
	db 0			; bits 23-16 of base
	db 0x9A			; access byte
	db 0xCF			; flags + bits 19-16 of limit
	db 0			; bits 31-24 of base
	; Kernel Data
	dw 0xFFFF
	dw 0
	db 0
	db 0x92
	db 0xCF
	db 0
	; User Code
	dw 0xFFFF
	dw 0
	db 0
	db 0xFA
	db 0xCF
	db 0
	; User Data
	dw 0xFFFF
	dw 0
	db 0
	db 0xF2
	db 0xCF
	db 0

done_msg db "Loading from Disk done! Switching to protected mode...", 0

times 510 - ($ - $$) db 0
dw 0xAA55
