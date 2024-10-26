ENTRY(_start)
OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:i386)

SECTIONS {
 . = 0x10000; /* start offset */
	.text ALIGN (4K) :
	{
		*(.text)
	}
	.rodata ALIGN (4K) :
	{
		*(.rodata)
	}
	.data ALIGN (4K) :
	{
		*(.data)
	}
	.bss ALIGN (4K) :
	{
		*(COMMON)
		*(.bss)
	}
	/* Add a symbol that indicates the end address of the program. */
 _prog_end = .;
}
