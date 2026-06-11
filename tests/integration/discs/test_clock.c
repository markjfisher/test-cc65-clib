/*
 * test_clock.c — tests clock() function on real BBC MOS
 * clock() calls OSWORD via beebium's real MOS implementation.
 */
#include <time.h>
#include <conio.h>

int main(void) {
    clock_t c;

    cputs("\r\n");

    c = clock();
    if (c != (clock_t)-1 && c > 0) {
        cputs("CLOCKOK\r\n");
    } else {
        cputs("CLOCK?\r\n");
    }

    return 0;
}