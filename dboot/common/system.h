#ifndef _SYSTEM_H_
#define _SYSTEM_H_

static void change_environment(unsigned char video_mode) {
	/* clear screen */
	__asm__ __volatile__ (
		"int $0x10" : : "a"(0x03)
	);
	/* set video mode */
	__asm__ __volatile__ (
		"int $0x10" : : "a"(0x0000 | video_mode)
	);
}

static void reboot_system(void) {
	/* far jump to 0xFFFF:0x0000 */
	__asm__ __volatile__ ("ljmpw $0xFFFF, $0x0000");
}

#endif
