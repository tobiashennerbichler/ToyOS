MEMORY {
	code 	 (rx) : ORIGIN = 0x200000, LENGTH = 8K
	vbe_info (r)  : ORIGIN = 0x202000, LENGTH = 256
	rom  	 (r)  : ORIGIN = 0x202100, LENGTH = 2K
	ram  	 (rw) : ORIGIN = 0x202900, LENGTH = 40K
}

SECTIONS {
	.text : {
		build/start.o(.text)
		*(.text)
	} > code

	.rodata : {
		*(.rodata*)
	} > rom

	.data : {
		*(.data)
	} > ram

	.bss : {
		. = ALIGN(16);
		_stack_end = .;
		build/start.o(.bss.stack)
		_stack_start = .;
	} > ram

	.vbe : {
		_vbe_mode_struct = .;
	} > vbe_info
}
