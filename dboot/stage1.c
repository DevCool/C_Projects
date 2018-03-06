__asm__(".code16gcc\n");
__asm__("jmpl $0x0000, $main\n");

#define MAX_COLS     320 /* maximum columns of the screen  */
#define MAX_ROWS     200 /* maximum rows of the screen     */

void print_string(const char* pStr) {
     while(*pStr) {
          __asm__ __volatile__ (
               "int $0x10" : : "a"(0x0e00 | *pStr), "b"(0x0007)
          );
          ++pStr;
     }
}

char getch() {
     char ch;
     __asm__ __volatile__ (
          "int $0x16\n"
          : "=a"(ch) : "a"(0x0001)
     );
     return ch;
}

void draw_pixel(unsigned char color, int col, int row) {
     __asm__ __volatile__ (
          "int $0x10" : : "a"(0x0c00 | color), "c"(col), "d"(row)
     );
}

void change_environment(unsigned char video_mode) {
     /* clear screen */
     __asm__ __volatile__ (
          "int $0x10" : : "a"(0x03)
     );
     __asm__ __volatile__ (
          "int $0x10" : : "a"(0x0000 | video_mode)
     );
}

void init_graphics() {
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

void main() {
     char ch;
     print_string("Hit 'g' key for graphics.\r\n");
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