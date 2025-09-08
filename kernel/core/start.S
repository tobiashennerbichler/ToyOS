.intel_syntax noprefix

.section .text
.extern kmain
.global _start
.type _start, @function

_start:
	lea esp, [_stack_start] # mov esp, OFFSET _stack_start
	call kmain
	hlt

.section .bss.stack
	.skip 16384
