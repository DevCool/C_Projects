#include "code16gcc.h"
#include "io.h"
#include "system.h"
#include "disk.h"

void main(void)
{
	print("Loading sector from floppy.\r\n");
	read_disk();
	print("Press a key to reboot.\r\n");
	reboot();
}

