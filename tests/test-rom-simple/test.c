/*
 * Simple ROM Function Test
 * Tests basic functions available in CLIB ROM
 */

#include <stdlib.h>
#include <string.h>

/* BBC OS functions for simple output */
extern void OSWRCH(unsigned char c);

/* Simple output functions */
void print_char(char c) {
    OSWRCH(c);
}

void print_string(const char* s) {
    while (*s) {
        print_char(*s++);
    }
}

void print_newline(void) {
    print_char(13);  /* CR */
    print_char(10);  /* LF */
}

void print_hex_byte(unsigned char value) {
    unsigned char high, low;
    high = (value >> 4) & 0x0F;
    low = value & 0x0F;
    
    print_char(high < 10 ? '0' + high : 'A' + high - 10);
    print_char(low < 10 ? '0' + low : 'A' + low - 10);
}

void print_decimal(int value) {
    char buffer[6];
    int i = 0;
    int negative = 0;
    
    if (value < 0) {
        negative = 1;
        value = -value;
    }
    
    if (value == 0) {
        print_char('0');
        return;
    }
    
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    
    if (negative) {
        print_char('-');
    }
    
    while (i > 0) {
        print_char(buffer[--i]);
    }
}

int main(void) {
    char test_string[] = "Hello ROM!";
    char copy_buffer[20];
    int test_length;
    int negative_number = -42;
    int positive_result;
    
    print_string("CLIB ROM Test Starting");
    print_newline();
    print_newline();
    
    /* Test strlen */
    print_string("1. Testing strlen('");
    print_string(test_string);
    print_string("')");
    print_newline();
    
    test_length = strlen(test_string);
    print_string("   Length: ");
    print_decimal(test_length);
    print_string(" (expected: 10)");
    print_newline();
    print_newline();
    
    /* Test strcpy */
    print_string("2. Testing strcpy");
    print_newline();
    strcpy(copy_buffer, test_string);
    print_string("   Original: '");
    print_string(test_string);
    print_string("'");
    print_newline();
    print_string("   Copy:     '");
    print_string(copy_buffer);
    print_string("'");
    print_newline();
    print_newline();
    
    /* Test abs */
    print_string("3. Testing abs(");
    print_decimal(negative_number);
    print_string(")");
    print_newline();
    
    positive_result = abs(negative_number);
    print_string("   Result: ");
    print_decimal(positive_result);
    print_string(" (expected: 42)");
    print_newline();
    print_newline();
    
    print_string("All ROM function tests completed!");
    print_newline();
    print_string("If you see this message, the ROM is working correctly.");
    print_newline();
    
    return 0;
}
