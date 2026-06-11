/*
 * test_clock.c — tests clock() function
 */
#include <time.h>
#include <conio.h>

int main(void) {
    clock_t start, end;

    start = clock();
    {
        volatile int i;
        for (i = 0; i < 1000; i++) ;
    }
    end = clock();

    if (end > start) {
        cputs("CLOCKOK");
    } else {
        cputs("CLOCKBAD");
    }

    return 0;
}