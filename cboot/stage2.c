#include "code16gcc.h"
__asm__("jmpl $0x0000, $main\n");

#include "funcs.h"

void main(void)
{
	unsigned char ch;
	for (;;) {
		print("Press 'q' to reboot system...\r\n"
			"Press 'e' to wipe CMOS!\r\n");
		ch = getch();
		switch (ch) {
		case 'q':
		case 'Q':
			reboot();
			break;
		case 'e':
		case 'E':
			print("Wiping CMOS...\r\n");
			clear_cmos();
			break;
		default:
			print("Invalid key pressed\r\n");
			break;
		}
	}
}

