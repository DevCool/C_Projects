#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifndef _CODE16GCC_H_
#include "code16gcc.h"
#endif

static inline void reboot(void)
{
	__asm__ __volatile__(
		"ljmpw $0xFFFF, $0x0000;"
	);
}

static inline void clear_cmos(void)
{
	unsigned char i = 0;
	while (i++ <= 255) {
		__asm__ __volatile__(
			"xor %ax, %ax; \
			in $70, %ax; \
			out %ax, $71;"
		);
	}
}

#endif
