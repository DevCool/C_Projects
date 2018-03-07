#ifndef _IO_H_
#define _IO_H_

static void print_string_color(const char* pStr, unsigned char color) {
     while(*pStr) {
		__asm__ __volatile__(
			"int $0x10"
			: : "a"(0x0E00 | *pStr++), "b"(0x0000 | color)
		);
     }
}
#define print_string(M) print_string_color(M, 0x07)

static char getch() {
     char ch;
     __asm__ __volatile__ (
          "int $0x16\n"
          : "=a"(ch) : "a"(0x0001)
     );
     return ch;
}

#endif
