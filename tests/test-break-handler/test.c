#include <stdio.h>
#include <stdint.h>
#include <conio.h>   /* cgetc() */

/* Public C ABI from the library */
extern unsigned char __fastcall__ set_brk_ret(void);
extern unsigned char __fastcall__ set_brk_ret_debug(void);
extern void          __fastcall__ disarm_brk_ret(void);
extern void          __fastcall__ set_brk_debug_mode_only(void);

/* BRK helpers (from brk_trigger.s) */
extern void cause_brk_non_esc(void);
extern void cause_brk_esc(void);

static unsigned char (*install_guard)(void) = 0;   /* prod or debug installer */

static void wait_key(const char* prompt) {
    if (prompt) printf("%s", prompt);
    cgetc();
    printf("\n");
}

static int choose_mode(void) {
    char ch = 0;
    printf("Select installer mode:\n");
    printf("  [P] Production  (pass-through -> OS)\n");
    printf("  [D] Debug       (pass-through -> bomb banner + hang)\n");
    printf("  [Q] Quit\n");
    printf("> ");
    while (ch = cgetc()) {
        printf("You pressed: %c\n", ch);
        if (ch == 'p' || ch == 'P') { install_guard = set_brk_ret;          printf("Production selected.\n\n"); break; }
        if (ch == 'd' || ch == 'D') { install_guard = set_brk_ret_debug;    printf("Debug selected.\n\n");
                                      set_brk_debug_mode_only();
                                      break; }
        if (ch == 'q' || ch == 'Q') return -1;
    }
    return 0;
}

/* Test 1: Armed + non-ESC BRK -> should long-jump, return A=1, app recovers */
static void test_armed_nonesc(void) {
    unsigned char r;
    printf("Test 1: Armed + non-ESC BRK\n");
    printf("Expected: application RECOVERS (A=1), returns to menu.\n");
    r = install_guard();
    if (r == 0) {
        printf("  Guard armed (A=0). Triggering BRK [$02]... ");
        cause_brk_non_esc();
        printf("\nERROR: fell through after BRK (should have long-jumped)!\n");
        disarm_brk_ret();
        return;
    } else {
        printf("OK (A=1). Disarming...\n");
        disarm_brk_ret();
        printf("Done.\n\n");
    }
}

/* Test 2: Armed + ESC BRK -> pass-through
   - Production: exits via OS routine (graceful exit)
   - Debug: prints bomb banner and HANGS
*/
static void test_armed_esc(void) {
    unsigned char r;
    printf("Test 2: Armed + ESC BRK\n");
    printf("Expected: PASS-THROUGH.\n");
    printf("  Production: graceful OS exit (program returns to OS).\n");
    printf("  Debug: bomb banner then HANG (program won't return).\n");
    r = install_guard();
    if (r == 0) {
        wait_key("Press a key to trigger ESC BRK [$1B]...");
        cause_brk_esc();
        /* We should never get here */
        printf("ERROR: fell through after ESC BRK (pass-through expected)!\n");
        disarm_brk_ret();
        return;
    } else {
        /* Armed path should never yield A=1 on ESC */
        printf("Unexpected: returned via armed path (A=%u)\n", (unsigned)r);
        disarm_brk_ret();
    }
}

/* Test 3: UNARMED + non-ESC BRK -> pass-through
   - Production: graceful OS exit
   - Debug: bomb banner + hang
*/
static void test_unarmed_nonesc(void) {
    printf("Test 3: UNARMED + non-ESC BRK\n");
    printf("Expected: PASS-THROUGH.\n");
    printf("  Production: graceful OS exit (program returns to OS).\n");
    printf("  Debug: bomb banner then HANG (program won't return).\n");
    wait_key("Press a key to trigger BRK [$02] unarmed… ");
    cause_brk_non_esc();
    printf("ERROR: fell through after unarmed BRK (pass-through expected)!\n");
}

/* Test 4: UNARMED + ESC BRK -> pass-through (same outcomes as Test 3) */
static void test_unarmed_esc(void) {
    printf("Test 4: UNARMED + ESC BRK\n");
    printf("Expected: PASS-THROUGH.\n");
    printf("  Production: graceful OS exit (program returns to OS).\n");
    printf("  Debug: bomb banner then HANG (program won't return).\n");
    wait_key("Press a key to trigger ESC BRK [$1B] unarmed… ");
    cause_brk_esc();
    printf("ERROR: fell through after unarmed ESC BRK (pass-through expected)!\n");
}

int main(void) {
    char choice;
    int r;
    r = choose_mode();
    if (r == -1) {
        printf("Exiting\n");
        return 0;
    }

    for (;;) {
        printf("BRK test menu (%s):\n", (install_guard == set_brk_ret) ? "Production" : "Debug");
        printf("  1) Armed + non-ESC BRK  (should recover, A=1)\n");
        printf("  2) Armed + ESC BRK      (pass-through: OS or banner+hang)\n");
        printf("  3) Unarmed + non-ESC    (pass-through: OS or banner+hang)\n");
        printf("  4) Unarmed + ESC        (pass-through: OS or banner+hang)\n");
        printf("  q) Quit\n");
        printf("> ");

        choice = cgetc();
        printf("%c\n", choice);

        switch (choice) {
            case '1': test_armed_nonesc(); break;
            case '2': test_armed_esc();    break;
            case '3': test_unarmed_nonesc(); break;
            case '4': test_unarmed_esc();    break;
            case 'q':
            case 'Q':
                printf("Bye.\n");
                return 0;
            default:
                printf("Unknown choice.\n");
        }
    }
}
