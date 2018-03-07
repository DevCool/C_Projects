#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

static void draw_pixel(unsigned char color, int col, int row) {
	/* draws pixel at specific location */
	__asm__ __volatile__ (
		"int $0x10" : : "a"(0x0c00 | color), "c"(col), "d"(row)
	);
}

static void init_graphics() {
	int i = 0, j = 0;
	int m = 0;
	int cnt1 = 0, cnt2 =0;
	unsigned char color = 10;

	for(;;) {
		if(m < (MAX_ROWS - m)) {
			++cnt1;
		}
		if(m < (MAX_COLS - m - 3)) {
			++cnt2;
		}

		if(cnt1 != cnt2) {
			cnt1  = 0;
			cnt2  = 0;
			m     = 0;
			if(++color > 255) color= 0;
		}

		/* (left, top) to (left, bottom)             */
		j = 0;
		for(i = m; i < MAX_ROWS - m; ++i) {
			draw_pixel(color, j+m, i);
		}
		/* (left, bottom) to (right, bottom)         */
		for(j = m; j < MAX_COLS - m; ++j) {
			draw_pixel(color, j, i);
		}

		/* (right, bottom) to (right, top)           */
		for(i = MAX_ROWS - m - 1 ; i >= m; --i) {
			draw_pixel(color, MAX_COLS - m - 1, i);
		}
		/* (right, top)   to (left, top)             */
		for(j = MAX_COLS - m - 1; j >= m; --j) {
			draw_pixel(color, j, m);
		}
		m += 6;
		if(++color > 255)  color = 0;
	}
}

#endif
