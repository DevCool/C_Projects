#include "code16gcc.h"
__asm__("jmp main");
#include "io.h"

int main()
{
	unsigned char ch, i;
	for (;;) {
		print("Press 'q' to reboot system...\r\n"
			"Press 'e' to wipe CMOS!\r\n");
		ch = getch();
		switch (ch) {
		case 'q':
		case 'Q':
			__asm__("jmp $0xFFFF, $0x0000;");
			break;
		case 'e':
		case 'E':
			print("Wiping CMOS...\r\n");
			for (i = 0; i < 255; i++)
				__asm__ __volatile__(
					"xor %ax, %ax;"
					"in $0x70, %ax;"
					"out %ax, $0x71;"
				);
			break;
		default:
			print("Invalid key pressed\r\n");
			break;
		}
	}
}

