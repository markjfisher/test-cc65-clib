/*
 * test_screen.c — tests full screen mode and scrolling with cputc/cputs
 */
#include <conio.h>

int main(void) {
    unsigned char i;

    clrscr();

    /* Print some lines to test scrolling */
    for (i = 0; i < 5; i++) {
        cputc('0' + i);
        cputs(": LINE\r\n");
    }

    /* Cursor positioning after scroll */
    gotoxy(0, 10);
    cputs("MODE7OK");

    return 0;
}