/*
 * test_screen.c — tests screen output scrolling and cursor positioning
 */
#include <conio.h>

int main(void) {
    unsigned char i;

    cputs("\r\n");

    for (i = 0; i < 5; i++) {
        cputc('0' + i);
        cputs(": LINE\r\n");
    }

    gotoxy(0, 10);
    cputs("MODE7OK\r\n");

    return 0;
}