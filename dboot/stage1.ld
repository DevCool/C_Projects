ENTRY(main);
SECTIONS
{
	. = 0x7c00;
	.text : AT(0x7c00)
	{
		_text = .;
		*(.text);
		_text_end = .;
	}
	.sig : AT(0x7dfe)
	{
		SHORT(0xaa55);
	}
	/DISCARD/ :
	{
		*(.note*);
		*(.iplt*);
		*(.igot*);
		*(.rel*);
		*(.comment);
		*(.eh_frame);
	}
}
