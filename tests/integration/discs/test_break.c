/*
 * test_break.c — tests BRK handler via set_brk_ret and disarm_brk_ret
 * Note: cputs is not available in the bare harness for break handler tests
 * because it uses OSWRCH which is MOS-based. In beebium integration tests
 * the real MOS handles OSWRCH properly.
 */
#include <conio.h>

extern unsigned char __fastcall__ set_brk_ret(void);
extern void          __fastcall__ disarm_brk_ret(void);
extern void          cause_brk_func(void);

int main(void) {
    unsigned char r;

    cputs("\r\n");

    r = set_brk_ret();
    if (r != 0) {
        cputs("ARMBAD\r\n");
        return 1;
    }

    cause_brk_func();

    disarm_brk_ret();
    cputs("BRKOK\r\n");

    return 0;
}