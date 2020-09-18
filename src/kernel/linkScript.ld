/*  ===== Date Created: 12 September, 2020 =====  */
ENTRY(start)

SECTIONS
{
	/* defined as KERNEL_FLAT_ADDRESS in src/kernel/macros-inl.asm */
	. = 0x20000;

	.entry BLOCK(4K) : ALIGN(4K)
	{
		*(.entry)
	}

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	
	/* Will not be included in the output file */
	/DISCARD/ :
	{
		*(.fini_array)
		*(.comment)
	}
}
