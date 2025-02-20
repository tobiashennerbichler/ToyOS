SECTIONS {
	. = 2M;
	.text : {
		src/start.o(.text)
		*(.text)
	}

	.bss : {
		. = ALIGN(16);
		_stack_bottom = .;
		*(.bss)
		_stack_top = .;
	}
}
