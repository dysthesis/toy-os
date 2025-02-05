/* The program should start at the `boot()` function */
ENTRY(boot)

/* The sections will be arranged in memory in the order of their appearance
		below. */
SECTIONS {
	/* Define the base address.
			This symbol also serves to represent the current address. */
	. = 0x80200000;

	/* This section goes first... */
	/* NOTE: this section contains the code of the program. */
	.text :{
		/* .text.boot is always placed at the beginning of the memory */
		KEEP(*(.text.boot));
		*(.text .text.*);
	}

	/* ...this one second... */
	/* NOTE: this section contains constant data. */
	.rodata : ALIGN(4) {
		*(.rodata .rodata.*)
	}

	/* ...this one third... */
	/* NOTE: this section contains read/write data. */
	.data : ALIGN(4) {
		*(.data .data.*)
	}

	/* ...and this last. */
	/* NOTE: this section contains read/write data with an initial value of zero. */
	.bss : ALIGN(4) {
		__bss = .;
		*(.bss .bss.* .sbss .sbss*);
		__bss_end = .;
	}

	/* After .bss comes the stack, which we define to be
	128 kB in size. */
	/* NOTE: this will actually be defined in asm in the `boot()` function in `kernel.c` */
	. = ALIGN(4);			/* Align the current address to the 4 byte boundary... */
	. += 128 * 1024;	/* ...add 128 kB to it... */
	__stack_top = .;		/* ...and declare it as the top of the stack. */
}
