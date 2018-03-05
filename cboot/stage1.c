#include "code16gcc.h"
__asm__("jmpl $0x0000, $boot_main\n\t");

#include "funcs.h"

void boot_main(void)
{
	print("Loading sector from floppy.\r\n");
	read_sector();
}

