BOOT_CODE_ADDRESS		equ	0x7C00
RELOC_KERNEL_ADDRESS	equ	0x200000
RELOC_VBE_MODE_STRUCT	equ 0x202000
NUM_SECTORS				equ	79		; If NUM_SECTORS*SECTOR_SIZE becomes > 65535 (0xFFFF) we need to perform multiple loads (offset overflow in disk packet)
SECTOR_SIZE				equ	512

WIDTH					equ	1024
HEIGHT					equ 768
BPP						equ 32

;=============================BOOT SECTOR=============================

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
	jmp $

VESA_error:
	mov si, vesa_err_msg
	call print_string
	jmp $

init:
	cli
	xor ax, ax
	mov ss, ax
	mov sp, BOOT_CODE_ADDRESS
	mov ds, ax
	mov es, ax
	mov fs, ax
	sti

	cld

load_from_disk:
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

enable_VESA:
	; Get VESA BIOS information
	mov ax, 0x4F00
	mov di, vbe_struct
	int 0x10
	cmp ax, 0x4F
	jne VESA_error

	mov si, [vbe_struct.vid_mode_off]
	mov ax, [vbe_struct.vid_mode_seg]
	mov fs, ax

	.iterate_modes:
	mov cx, [fs:si]				; Get next mode
	add si, 2
	cmp cx, 0xFFFF
	je VESA_error				; Desired mode not included in list

	; Get mode info
	mov ax, 0x4F01
	mov di, vbe_mode_struct
	int 0x10
	cmp ax, 0x4F
	jne VESA_error

	; Check width
	mov ax, [es:vbe_mode_struct.width]
	cmp ax, WIDTH
	jne .iterate_modes

	; Check height
	mov ax, [es:vbe_mode_struct.height]
	cmp ax, HEIGHT
	jne .iterate_modes

	; Check bpp
	mov al, [es:vbe_mode_struct.bpp]
	cmp al, BPP
	jne .iterate_modes

	; Set VBE mode
	mov ax, 0x4F02
	mov bx, cx
	or bx, 0x4000
	int 0x10
	cmp ax, 0x4F
	jne VESA_error

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

	; Load GDT
	.load_gdt:
	lgdt [gdt]

switch:
	; Switch to protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; Far Jump to load kernel code segment selector into cs
	jmp 0x08:protected

[BITS 32]
protected:
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax

	; Relocate vbe mode struct into address known by linker
	mov esi, vbe_mode_struct
	mov edi, RELOC_VBE_MODE_STRUCT
	mov ecx, 256
	rep movsb

	; Relocate code to high memory and jump to _start function
	mov esi, kernel_start
	mov edi, RELOC_KERNEL_ADDRESS
	mov ecx, NUM_SECTORS*SECTOR_SIZE
	rep movsb
	jmp RELOC_KERNEL_ADDRESS

disk_packet:
	db 16
	db 0
	dw NUM_SECTORS			; num sectors
	dw 0					; offset
	dw 0x7E0				; segment address
	dd 1					; lower 32-bit LDA address
	dd 0					; upper 16-bit LDA address

gdt:
	; Null Entry - use it as placeholder to be used with lgdt instruction
	dw end_gdt - gdt - 1	; size of table - 1
	dd gdt					; address of this table
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
end_gdt:

sgdt:
	dw 0
	dd 0

vesa_err_msg db "VESA not supported", 0

times 510 - ($ - $$) db 0
dw 0xAA55
;=============================BOOT SECTOR END=============================

;=============================SECOND SECTOR=============================
vbe_mode_struct:
	.attributes		dw 0
	.unused1		times 16 db 0
	.width			dw 0
	.height			dw 0
	.unused2		times 3 db 0
	.bpp			db 0
	.unused3		times 230 db 0

times 512 - ($ - vbe_mode_struct) db 0
;=============================SECOND SECTOR END=============================

;=============================THIRD SECTOR=============================
; Already 512 bytes
vbe_struct:
	.signature		db "VBE2"
	.unused1		times 10 db 0
	.vid_mode_off	dw 0
	.vid_mode_seg	dw 0
	.unused2		times 494 db 0
;=============================THIRD SECTOR END=============================

;=============================KERNEL LOADED AFTERWARDS=============================
kernel_start: