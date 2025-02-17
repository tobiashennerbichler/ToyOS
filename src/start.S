.section .text
.extern main
.global _start
.type _start, @function

_start:
	xor %ax, %ax
	mov %ax, %ss
	mov %ax, %ds
	mov %ax, %es
	mov stack_bottom, %esp

	call main

.section .bss
.align 4
stack_bottom:
	.skip 16384
stack_top:
