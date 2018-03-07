#include "code16gcc.h"
#include "io.h"
#include "system.h"

int main(void)
{
	unsigned char cf, i, reset;
	print("Loading sector from floppy.\r\n");
	for (i = 0; i < 3; i++) {
		cf = 0;
		__asm__(
			"int $0x13; \
			setc %0;"
			: "=r"(cf)
			: "a"(0x0202), "b"(0x1000), "c"(0x0002), "d"(0x0000)
		);
		if (cf) {
			reset = 1;
			print("Could not read the disk"
				" sector.\r\n");
			while (reset)
				__asm__(
					"mov $0x00, %%ah;"
					"int $0x13;"
					"or %%ah, %%ah;"
					"mov %%ah, %0;"
					: "=r"(reset)
					:
				);
		} else {
			print("Sector read.\r\n");
			__asm__(
				"cli;"
				"xor %ax, %ax;"
				"mov %ss, %ax;"
				"mov $0x1000, %sp;"
				"jmp $0x0000, $0x1000;"
				"popa;"
			);
			break;
		}
	}
	print("Press a key to reboot.\r\n");
	reboot();
}

