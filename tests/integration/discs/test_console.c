/*
 * test_console.c — tests console I/O functions
 */
#include <conio.h>

int main(void) {
    unsigned char x, y;

    clrscr();

    cputc('A');
    cputc('B');
    cputc('C');
    cputc('\r');
    cputc('\n');

    gotoxy(10, 5);
    x = wherex();
    y = wherey();

    cputs("XY:");
    if (x >= 10) cputc('0' + (x / 10)); else cputc('0');
    cputc('0' + (x % 10));
    cputc(',');
    if (y >= 10) cputc('0' + (y / 10)); else cputc('0');
    cputc('0' + (y % 10));
    cputc('\r');
    cputc('\n');

    textcolor(1);
    bordercolor(4);
    cputs("COLOROK");

    return 0;
}