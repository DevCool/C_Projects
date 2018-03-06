#include "code16gcc.h"
__asm__("jmpl $0, $boot_main\n");

void putch(char ch)
{
	__asm__ __volatile__(
		"pusha;"
		"mov $0x0E, %%ah;"
		"mov %0, %%al;"
		"mov $0x0007, %%bx;"
		"mov $1, %%cx;"
		"int $0x10;"
		"popa;"
		:
		: "r"(ch)
	);
}

void print(const char *s)
{
	while (*s) {
		putch(*s++);
	}
}

/*
char getche(void)
{
	unsigned char ch;
	__asm__ __volatile__(
		"mov $0x00, %%ah;"
		"mov %%al, %0;"
		"int $0x16;"
		: "=r"(ch)
		:
	);
	__asm__ __volatile__(
		"mov $0x0E, %%ah;"
		"mov %%al, %0;"
		"mov $0x0A, %%bl;"
		"int $0x10;"
		:
		: "r"(ch)
	);
	return ch;
}

char getch(void)
{
	char ch;
	__asm__ __volatile__(
		"mov $0x00, %%ah;"
		"mov %%al, %0;"
		"int $0x16;"
		: "=r"(ch)
		:
	);
	return ch;
}

void reboot(void)
{
	__asm__(
		"jmp $0xFFFF, $0x0000\n"
	);
}

void clear_cmos(void)
{
	unsigned char i = 0;
	while (i++ <= 255) {
		__asm__ __volatile__(
			"xor %ax, %ax;"
			"in $70, %ax;"
			"out %ax, $71;"
		);
	}
}

void init_graphics(void)
{
	__asm__(
		"mov $0x0003, %ax;"
		"int $0x10;"
		"mov $0x0013, %ax;"
		"int $0x10;"
	);
}

void draw_pixel(unsigned short x, unsigned short y, unsigned char color)
{
	__asm__ __volatile__(
		"pusha;"
		"mov $0x0c, %%ah;"
		"mov $0x00, %%bh;"
		"mov %0, %%bl;"
		"mov %1, %%dx;"
		"mov %2, %%cx;"
		"mov $0x04, %%al;"
		"int $0x10;"
		"popa;"
		:
		: "r"(color), "r"(y), "r"(x)
	);
}

void boot_main(void)
{
	short i, j;
	init_graphics();
	for (i = 0; i < 320; i++) {
		for (j = 0; j < 320; j++)
			draw_pixel(0, j++, 0x0A);
		draw_pixel(i++, 0, 0x0A);
	}
	getch();
	__asm__("jmp $0xFFFF, $0x0000;");
}
*/

void boot_main(void)
{
	unsigned char cf, i, reset;
	print("Loading sector from floppy.\r\n");
	for (i = 0; i < 3; i++) {
		cf = 0;
		__asm__(
			"pusha;"
			"mov $0x0, %%ax;"
			"mov %%ax, %%es;"
			"mov $0x1000, %%bx;"
			"mov $0x02, %%ah;"
			"mov $0x02, %%al;"
			"mov $0x00, %%ch;" /* track/cylinder */
			"mov $0x02, %%cl;" /* sector to read */
			"mov $0x00, %%dh;" /* head number */
			"mov $0x00, %%dl;" /* drive number. */
			"int $0x13;"	/* call BIOS; read sector */
			"setc %0;"
			"popa;"
			: "=r"(cf)
			:
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
}

