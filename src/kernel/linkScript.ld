/*  ===== Date Created: 03 December, 2020 =====  */
ENTRY(start)

SECTIONS
{
	HIGHER_HALF_OFFSET = 0xc0000000;

	/* The kernel begins at the 1MB physical mark. */
	. = HIGHER_HALF_OFFSET + 0x00100000;

	.text ALIGN(4k) : AT(ADDR(.text) - HIGHER_HALF_OFFSET)
	{
		*(.entry)
		*(.text .text.*)
	}

	.rodata ALIGN(4k) : AT(ADDR(.rodata) - HIGHER_HALF_OFFSET)
	{
		*(.rodata .rodata.*)
	}

	.data ALIGN(4k) : AT(ADDR(.data) - HIGHER_HALF_OFFSET)
	{
		*(.data .data.*)
	}

	.magic ALIGN(4k) : AT(ADDR(.magic) - HIGHER_HALF_OFFSET)
	{
		*(.magic)
	}

	/* Not actually *in* the image. */
	.bss ALIGN(4k) : AT(ADDR(.bss) - HIGHER_HALF_OFFSET)
	{
		bssBlockStart = .;
		*(COMMON)
		*(.bss .bss.*)
		bssBlockEnd = .;
	}
}
