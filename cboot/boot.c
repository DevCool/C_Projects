#include "code16gcc.h"
__asm__("jmpl $0x0000, $boot_main\n\t");

#include "funcs.h"

void boot_main(void)
{
	unsigned char cf, i, reset;
	print("Loading sector from floppy.\r\n");
	for (i = 0; i < 3; i++) {
		cf = 0;
		__asm__(
			"pusha\n\t"
			"mov $0x0, %%ax\n\t"
			"mov %%ax, %%es\n\t"
			"mov $0x1000, %%bx\n\t"
			"mov $0x02, %%ah\n\t"
			"mov $0x02, %%al\n\t"
			"mov $0x00, %%ch\n\t" /* track/cylinder */
			"mov $0x02, %%cl\n\t" /* sector to read */
			"mov $0x00, %%dh\n\t" /* head number */
			"mov $0x00, %%dl\n\t" /* drive number. */
			"int $0x13\n\t"	/* call BIOS; read sector */
			"setc %0\n\t"
			"popa\n\t"
			: "=r"(cf)
			:
		);
		if (cf) {
			reset = 1;
			print("Could not read the disk"
				" sector.\r\n");
			while (reset)
				__asm__(
					"mov $0x00, %%ah\n\t"
					"int $0x13\n\t"
					"or %%ah, %%ah\n\t"
					"mov %%ah, %0\n\t"
					: "=r"(reset)
					:
				);
		} else {
			print("Sector read.\r\n");
			__asm__(
				"pusha\n\t"
				"xor %ax, %ax\n\t"
				"mov %ss, %ax\n\t"
				"mov $0x1000, %sp\n\t"
				"jmpl $0x0000, $0x1000\n\t"
				"popa\n\t"
			);
			break;
		}
	}
}

