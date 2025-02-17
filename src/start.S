.intel_syntax noprefix

.section .text
.extern main
.global _start
.type _start, @function

_start:
	xor ax, ax
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov esp, stack_top

	mov ebx, 0x77666677

	call main

.section .bss
stack_bottom:
	.skip 16384
stack_top:
