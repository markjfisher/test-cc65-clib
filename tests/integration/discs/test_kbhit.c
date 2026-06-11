/*
 * test_kbhit.c — tests kbhit and cgetc on the BBC
 * beebium injects a keypress after the program starts
 */
#include <conio.h>

int main(void) {
    unsigned char ch;

    cputs("\r\nPRESS A\r\n");
    ch = cgetc();
    cputc(ch);
    cputs(" OK\r\n");

    return 0;
}