/*
 * Comprehensive C Library ROM Test  
 * Tests wide range of functions to validate ROM/local function splitting
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* BBC OS functions for simple output */
void __fastcall__ OSWRCH(unsigned char c);

/* BBC OS read character - direct ROM call at FFE0 */
char (*OSRDCH)(void) = (char (*)(void))0xFFE0;

/* Simple output functions */
void print_char(char c) {
    OSWRCH(c);
}

void print_string(const char* s) {
    while (*s) {
        print_char(*s++);
    }
}

void wait_for_key(void) {
    print_string("\r\n--- Press any key to continue ---\r\n");
    OSRDCH();  /* Direct BBC OS call */
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
    char copy_buffer[50];
    char work_buffer[50]; 
    char memory_buffer[20];
    int test_length, result, i;
    int negative_number = -42;
    int positive_result;
    int math_a = 15, math_b = 3;
    
    print_string("CLIB ROM Test Starting");
    print_newline();
    print_string("Testing ROM vs Local fn separation");
    print_newline();
    print_newline();
    
    /* === STRING FUNCTIONS === */
    print_string("=== STRING FUNCTIONS ===");
    print_newline();
    
    /* Test strlen */
    print_string("1. 'Hello ROM!' strlen: ");
    test_length = strlen(test_string);
    print_decimal(test_length);
    print_string(" chars");
    print_newline();
    
    /* Test strcpy */
    print_string("2. strcpy: ");
    strcpy(copy_buffer, test_string);
    print_string("copied '");
    print_string(copy_buffer);
    print_string("'");
    print_newline();
    
    /* Test strcat */
    print_string("3. strcat: ");
    strcat(copy_buffer, " World");
    print_string("result '");
    print_string(copy_buffer);
    print_string("'");
    print_newline();
    
    /* Test strcmp */
    print_string("4. strcmp: ");
    result = strcmp("ABC", "ABC");
    print_string("'ABC'=='ABC' = ");
    print_decimal(result);
    print_string(" (== 0)");
    print_newline();

    print_string("b. strcmp: ");
    result = strcmp("ABC", "CDE");
    print_string("'ABC'=='CDE' = ");
    print_decimal(result);
    print_string(" (< 0)");
    print_newline();
    
    print_string("c. strcmp: ");
    result = strcmp("CDE", "ABC");
    print_string("'CDE'=='ABC' = ");
    print_decimal(result);
    print_string(" (> 0)");
    print_newline();
    
    /* Test strncpy */
    print_string("5. strncpy: ");
    strncpy(work_buffer, "TestingStrncpy", 7);
    work_buffer[7] = 0; /* null terminate */
    print_string("first 7 chars = '");
    print_string(work_buffer);
    print_string("'");
    print_newline();
    
    /* Test strchr */
    print_string("6. strchr: ");
    if (strchr(test_string, 'R')) {
        print_string("found 'R' in 'Hello ROM!'");
    } else {
        print_string("'R' not found");
    }
    print_newline();
    
    wait_for_key();
    
    /* === MEMORY FUNCTIONS === */
    print_string("=== MEMORY FUNCTIONS ===");
    print_newline();
    
    /* Test memcpy */
    print_string("7. memcpy: ");
    memcpy(memory_buffer, "MemTest", 7);
    memory_buffer[7] = 0;
    print_string("copied '");
    print_string(memory_buffer);
    print_string("'");
    print_newline();
    
    /* Test memset */
    print_string("8. memset: ");
    memset(memory_buffer, 'X', 5);
    memory_buffer[5] = 0;
    print_string("= '");
    print_string(memory_buffer);
    print_string("' (== 'XXXXX')");
    print_newline();
    
    /* Test memcmp */
    print_string("9. memcmp: ABC:ABC ");
    result = memcmp("ABC", "ABC", 3);
    print_string("result = ");
    print_decimal(result);
    print_string(" (== 0)");
    print_newline();
    
    print_string("b. memcmp: ABC:DEF ");
    result = memcmp("ABC", "DEF", 3);
    print_string("result = ");
    print_decimal(result);
    print_string(" (< 0)");
    print_newline();
    
    print_string("c. memcmp: DEF:ABC ");
    result = memcmp("DEF", "ABC", 3);
    print_string("result = ");
    print_decimal(result);
    print_string(" (> 0)");
    print_newline();
    
    wait_for_key();
    
    /* === CHARACTER FUNCTIONS === */
    print_string("=== CHARACTER FUNCTIONS ===");
    print_newline();
    
    /* Test isalpha */
    print_string("10. isalpha('A'): ");
    result = isalpha('A') ? 1 : 0;
    print_decimal(result);
    print_string(" (1=true)");
    print_newline();
    
    print_string(" b. isalpha('0'): ");
    result = isalpha('0') ? 1 : 0;
    print_decimal(result);
    print_string(" (0=false)");
    print_newline();
    
    /* Test isdigit */
    print_string("11. isdigit('5'): ");
    result = isdigit('5') ? 1 : 0;
    print_decimal(result);
    print_string(" (1=true)");
    print_newline();
    
    print_string(" b. isdigit('X'): ");
    result = isdigit('X') ? 1 : 0;
    print_decimal(result);
    print_string(" (0=false)");
    print_newline();
    
    /* Test toupper */
    print_string("12. toupper('a'): '");
    print_char(toupper('a'));
    print_string("' (should be 'A')");
    print_newline();
    
    /* Test tolower */
    print_string("13. tolower('Z'): '");
    print_char(tolower('Z'));
    print_string("' (should be 'z')");
    print_newline();
    
    wait_for_key();
    
    /* === MATH FUNCTIONS === */
    print_string("=== MATH FUNCTIONS ===");
    print_newline();
    
    /* Test abs */
    print_string("14. abs(-42): ");
    positive_result = abs(negative_number);
    print_decimal(positive_result);
    print_newline();
    
    /* Test multiplication (uses runtime functions) */
    print_string("15. 15 * 3 = ");
    result = math_a * math_b;
    print_decimal(result);
    print_newline();
    
    /* Test division (uses runtime functions) */
    print_string("16. 15 / 3 = ");
    result = math_a / math_b;
    print_decimal(result);
    print_newline();
    
    /* Test modulo (uses runtime functions) */
    print_string("17. 15 % 3 = ");
    result = math_a % math_b;
    print_decimal(result);
    print_newline();
    
    wait_for_key();
    
    /* === CONVERSION FUNCTIONS === */
    print_string("=== CONVERSION FUNCTIONS ===");
    print_newline();
    
    /* Test atoi */
    print_string("18. atoi('123'): ");
    result = atoi("123");
    print_decimal(result);
    print_newline();
    
    wait_for_key();
    
    print_string("=== TEST COMPLETE ===");
    print_newline();
    print_string("All functions tested");
    print_newline();
    
    return 42;
}
