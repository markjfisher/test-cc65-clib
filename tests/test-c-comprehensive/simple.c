/*
 * Minimal C test to check if C programs work at all
 */

#include <stdlib.h>

/* BBC OS function - declare as assembly function */
void __fastcall__ OSWRCH(unsigned char c);

/* Access c_sp directly for debugging */
unsigned int* c_sp_ptr = (unsigned int*)0x50;  /* C software stack pointer */

void print_char(char c) {
    OSWRCH(c);
}

void print_hex_word(unsigned int value) {
    unsigned char hi, lo, hi_h, hi_l, lo_h, lo_l;
    
    hi = (value >> 8) & 0xFF;
    lo = value & 0xFF;
    
    /* Print high byte */
    hi_h = (hi >> 4) & 0x0F;
    hi_l = hi & 0x0F;
    print_char((hi_h < 10) ? ('0' + hi_h) : ('A' + hi_h - 10));
    print_char((hi_l < 10) ? ('0' + hi_l) : ('A' + hi_l - 10));
    
    /* Print low byte */
    lo_h = (lo >> 4) & 0x0F;
    lo_l = lo & 0x0F;
    print_char((lo_h < 10) ? ('0' + lo_h) : ('A' + lo_h - 10));
    print_char((lo_l < 10) ? ('0' + lo_l) : ('A' + lo_l - 10));
}

void print_string(const char* s) {
    while (*s) {
        print_char(*s++);
    }
}

int main(void) {
    print_string("Hello from C! v1.3");
    print_char(13); print_char(10);  /* CRLF */
    
    print_string("c_sp at start: ");
    print_hex_word(*c_sp_ptr);
    print_char(13); print_char(10);  /* CRLF */
    
    print_string("Line 2: Still working");
    print_char(13); print_char(10);  /* CRLF */
    
    print_string("Line 3: About to return");
    print_char(13); print_char(10);  /* CRLF */
    
    print_string("c_sp before return: ");
    print_hex_word(*c_sp_ptr);
    print_char(13); print_char(10);  /* CRLF */
    
    print_string("Calling return 0...");
    print_char(13); print_char(10);  /* CRLF */
    
    return 0;
}
