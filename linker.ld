MEMORY {
	code 	 (rx) : ORIGIN = 0x200000							, LENGTH = 8K
	vbe_info (r)  : ORIGIN = ORIGIN(code) + LENGTH(code)		, LENGTH = 256
	rom  	 (r)  : ORIGIN = ORIGIN(vbe_info) + LENGTH(vbe_info), LENGTH = 4K
	ram  	 (rw) : ORIGIN = ORIGIN(rom) + LENGTH(rom)			, LENGTH = 40K
}

SECTIONS {
	.text : {
		build/core/start.o(.text)
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
		build/core/start.o(.bss.stack)
		_stack_start = .;
	} > ram

	.vbe : {
		_vbe_mode_struct = .;
	} > vbe_info
}
