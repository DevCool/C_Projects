#ifndef _FUNCS_H_
#define _FUNCS_H_

void putch(char ch)
{
	__asm__ __volatile__(
		"pusha\n\t"
		"mov $0x0E, %%ah\n\t"
		"mov %0, %%al\n\t"
		"mov $0x0007, %%bx\n\t"
		"mov $1, %%cx\n\t"
		"int $0x10\n\t"
		"popa\n\t"
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

static char getche(void)
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

static char getch(void)
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

static void reboot(void)
{
	__asm__(
		"jmp $0xFFFF, $0x0000\n"
	);
}

static void clear_cmos(void)
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

static void init_graphics(void)
{
	__asm__(
		"mov $0x0003, %ax;"
		"int $0x10;"
		"mov $0x0013, %ax;"
		"int $0x10;"
	);
}

static void draw_pixel(unsigned short x, unsigned short y, unsigned char color)
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

static char reset_disk(void)
{
	char reset;
	__asm__(
		"mov $0x00, %%ah\n\t"
		"int $0x13\n\t"
		"or %%ah, %%ah\n\t"
		"mov %%ah, %0\n\t"
		: "=r"(reset)
		:
	);
	return reset;
}

static void read_sector(void)
{
	unsigned char cf;
	cf = -1;
	while (cf) {
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
			print("Could not read the disk"
				" sector.\r\n");
			reset_disk();
		} else {
			print("Sector read.\r\n");
			__asm__(
				"pusha\n\t"
				"xor %ax, %ax\n\t"
				"mov %ss, %ax\n\t"
				"mov $0x1000, %sp\n\t"
				"jmp $0x0000, $0x1000\n\t"
				"popa\n\t"
			);
			break;
		}
	}
}

#endif
