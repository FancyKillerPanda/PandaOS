/*  ===== Date Created: 03 December, 2020 =====  */
ENTRY(start)

SECTIONS
{
	/* The kernel begins at the 1MB physical mark. */
	. = 0x00100000;

	.text BLOCK(4k) : ALIGN(4k)
	{
		*(.entry)
		*(.text .text.*)
	}

	.rodata BLOCK(4k) : ALIGN(4k)
	{
		*(.rodata .rodata.*)
	}

	.data BLOCK(4k) : ALIGN(4k)
	{
		*(.data .data.*)
	}

	.magic BLOCK(4k) : ALIGN(4k)
	{
		*(.magic)
	}

	/* Not actually *in* the image. */
	.bss BLOCK(4k) : ALIGN(4k)
	{
		*(COMMON)
		*(.bss .bss.*)
	}
}
