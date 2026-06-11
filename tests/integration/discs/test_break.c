/*
 * test_break.c — tests BRK handler via set_brk_ret and disarm_brk_ret
 */
#include <conio.h>

extern unsigned char __fastcall__ set_brk_ret(void);
extern void          __fastcall__ disarm_brk_ret(void);
extern void          cause_brk_func(void);

int main(void) {
    unsigned char r;

    r = set_brk_ret();
    if (r != 0) {
        cputs("ARMBAD");
        return 1;
    }

    cause_brk_func();

    disarm_brk_ret();
    cputs("BRKOK");

    return 0;
}