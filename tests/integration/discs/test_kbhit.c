/*
 * test_kbhit.c — tests kbhit and cgetc on the BBC
 * Must be run on real hardware — beebium can inject keypresses
 */
#include <conio.h>

int main(void) {
    cputs("PRESS A");
    /* Wait for a key */
    {
        unsigned char ch;
        ch = cgetc();
        cputc(ch);
        cputs(" OK");
    }
    return 0;
}