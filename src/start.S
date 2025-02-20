.intel_syntax noprefix

.section .text
.extern main
.global _start
.type _start, @function

_start:
	mov esp, _stack_top
	call main

print_string:
	mov si, msg
	xor bx, bx
	mov ah, 0xE
	.print_loop:
		lodsb
		test al, al
		jz .end_loop
		int 0x10
		jmp .print_loop
	.end_loop:
		ret

.section .rodata
msg:
	.asciz "Hello World from _start"

.section .bss
	.skip 16384
