/*
 * test_console.c — tests console I/O functions
 * Uses only core output functions that work on real BBC MOS:
 * cputc, cputs, gotoxy, textcolor — all via OSWRCH.
 */
#include <conio.h>

int main(void) {
    cputs("\r\n");

    cputc('A');
    cputc('B');
    cputc('C');
    cputs("\r\n");

    gotoxy(0, 5);
    cputs("POSITION\r\n");

    textcolor(1);
    cputs("COLOROK\r\n");

    return 0;
}