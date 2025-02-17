SECTIONS {
	. = 2M;
	.text : {
		*(.text)
	}

	.bss : {
		. = ALIGN(4);
		*(.bss)
	}
}
