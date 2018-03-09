#ifndef _DISK_H_
#define _DISK_H_

#ifndef _IO_H_
#include "io.h"
#endif

/* read_disk:  reads 2 sectors from floppy; starting at 2nd sector */
static inline void read_disk(void)
{
	unsigned char cf, reset;
	cf = 0;
	__asm__ __volatile__(
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
			__asm__ __volatile__(
				"mov $0x00, %%ah;"
				"int $0x13;"
				"or %%ah, %%ah;"
				"mov %%ah, %0;"
				: "=r"(reset)
				:
			);
	} else {
		print("Sector read.\r\n");
		__asm__("cli");
		__asm__ __volatile__(
			"mov %%ss, %%ax; \
			mov $0x1000, %%sp; \
			jmp $0x0000, $0x1000;"
			:
			: "a"(0x0000)
		);
		__asm__("sti");
	}
}

#endif
