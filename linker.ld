SECTIONS {
	. = 0x7E00;
	.text : {
		*(.text)
		*(.text*)
	}

	.rodata : {
		. = ALIGN(16);
		*(.rodata)
	}

	.bss : {
		. = ALIGN(16);
		_stack_bottom = .;
		*(.bss)
		_stack_top = .;
	}
}
