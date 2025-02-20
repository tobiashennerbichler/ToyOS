.intel_syntax noprefix

.section .text
.extern kmain
.global _start
.type _start, @function

_start:
	lea esp, [_stack_top] # mov esp, OFFSET _stack_top
	call kmain
	hlt

.section .bss
	.skip 16384
