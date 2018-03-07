#include "common/io.h"
#include "common/system.h"

#define MAX_COLS     320 /* maximum columns of the screen  */
#define MAX_ROWS     200 /* maximum rows of the screen     */

#include "common/graphics.h"

void main() {
	char ch;
	print_string("Hit 'G' for graphics.\r\n");
	ch = getch();
	if (ch == 'g' || ch == 'G') {
		change_environment(0x13);
		init_graphics();
	} else {
		change_environment(0x03);
		print_string("Halting system.\r\n");
	}
	__asm__ ("cli\nhlt\n");
}
