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

/* Global string constants for debugging */
char WAIT_KEY_MSG[] = "\r\n--- Press any key to continue ---\r\n";
char STARTING_MSG[] = "CLIB ROM Test Starting";
char ROM_SEP_MSG[] = "Testing ROM vs Local fn separation";
char STRING_FUNCS_HEADER[] = "=== STRING FUNCTIONS ===";
char MEMORY_FUNCS_HEADER[] = "=== MEMORY FUNCTIONS ===";
char CHAR_FUNCS_HEADER[] = "=== CHARACTER FUNCTIONS ===";
char MATH_FUNCS_HEADER[] = "=== MATH FUNCTIONS ===";
char CONVERT_FUNCS_HEADER[] = "=== CONVERSION FUNCTIONS ===";
char TEST_COMPLETE_HEADER[] = "=== TEST COMPLETE ===";
char ALL_FUNCS_TESTED_MSG[] = "All functions tested";

/* Test output strings */
char STRLEN_TEST_PREFIX[] = "1. 'Hello ROM!' strlen: ";
char CHARS_SUFFIX[] = " chars";
char STRCPY_TEST_PREFIX[] = "2. strcpy: ";
char COPIED_PREFIX[] = "copied '";
char QUOTE_SUFFIX[] = "'";
char STRCAT_TEST_PREFIX[] = "3. strcat: ";
char RESULT_PREFIX[] = "result '";
char STRCMP_TEST_PREFIX[] = "4. strcmp: ";
char STRCMP_ABC_ABC_MSG[] = "'ABC'=='ABC' = ";
char EQUAL_ZERO_SUFFIX[] = " (== 0)";
char STRCMP_B_PREFIX[] = "b. strcmp: ";
char STRCMP_ABC_CDE_MSG[] = "'ABC'=='CDE' = ";
char LESS_ZERO_SUFFIX[] = " (< 0)";
char STRCMP_C_PREFIX[] = "c. strcmp: ";
char STRCMP_CDE_ABC_MSG[] = "'CDE'=='ABC' = ";
char GREATER_ZERO_SUFFIX[] = " (> 0)";
char STRNCPY_TEST_PREFIX[] = "5. strncpy: ";
char FIRST_7_CHARS_PREFIX[] = "first 7 chars = '";
char STRCHR_TEST_PREFIX[] = "6. strchr: ";
char FOUND_R_MSG[] = "found 'R' in 'Hello ROM!'";
char R_NOT_FOUND_MSG[] = "'R' not found";

char MEMCPY_TEST_PREFIX[] = "7. memcpy: ";
char MEMSET_TEST_PREFIX[] = "8. memset: ";
char MEMSET_RESULT_PREFIX[] = "= '";
char MEMSET_EXPECTED_SUFFIX[] = "' (== 'XXXXX')";
char MEMCMP_ABC_ABC_PREFIX[] = "9. memcmp: ABC:ABC ";
char RESULT_EQUALS_PREFIX[] = "result = ";
char MEMCMP_ABC_DEF_PREFIX[] = "b. memcmp: ABC:DEF ";
char MEMCMP_DEF_ABC_PREFIX[] = "c. memcmp: DEF:ABC ";

char ISALPHA_A_PREFIX[] = "10. isalpha('A'): ";
char TRUE_SUFFIX[] = " (1=true)";
char ISALPHA_0_PREFIX[] = " b. isalpha('0'): ";
char FALSE_SUFFIX[] = " (0=false)";
char ISDIGIT_5_PREFIX[] = "11. isdigit('5'): ";
char ISDIGIT_X_PREFIX[] = " b. isdigit('X'): ";
char TOUPPER_A_PREFIX[] = "12. toupper('a'): '";
char SHOULD_BE_A_SUFFIX[] = "' (should be 'A')";
char TOLOWER_Z_PREFIX[] = "13. tolower('Z'): '";
char SHOULD_BE_Z_SUFFIX[] = "' (should be 'z')";

char ABS_TEST_PREFIX[] = "14. abs(-42): ";
char MULT_TEST_PREFIX[] = "15. 15 * 3 = ";
char DIV_TEST_PREFIX[] = "16. 15 / 3 = ";
char MOD_TEST_PREFIX[] = "17. 15 % 3 = ";

char ATOI_TEST_PREFIX[] = "18. atoi('123'): ";

/* Test data strings */
char test_string[] = "Hello ROM!";
char WORLD_SUFFIX[] = " World";
char TEST_ABC[] = "ABC";
char TEST_CDE[] = "CDE"; 
char TEST_DEF[] = "DEF";
char TESTING_STRNCPY[] = "TestingStrncpy";
char MEMTEST[] = "MemTest";
char ATOI_TEST_VALUE[] = "123";

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
    print_string(WAIT_KEY_MSG);
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
    char copy_buffer[50];
    char work_buffer[50]; 
    char memory_buffer[20];
    int test_length, result, i;
    int negative_number = -42;
    int positive_result;
    int math_a = 15, math_b = 3;
    
    print_string(STARTING_MSG);
    print_newline();
    print_string(ROM_SEP_MSG);
    print_newline();
    print_newline();
    
    /* === STRING FUNCTIONS === */
    print_string(STRING_FUNCS_HEADER);
    print_newline();
    
    /* Test strlen */
    print_string(STRLEN_TEST_PREFIX);
    test_length = strlen(test_string);
    print_decimal(test_length);
    print_string(CHARS_SUFFIX);
    print_newline();
    
    /* Test strcpy */
    print_string(STRCPY_TEST_PREFIX);
    strcpy(copy_buffer, test_string);
    print_string(COPIED_PREFIX);
    print_string(copy_buffer);
    print_string(QUOTE_SUFFIX);
    print_newline();
    
    /* Test strcat */
    print_string(STRCAT_TEST_PREFIX);
    strcat(copy_buffer, WORLD_SUFFIX);
    print_string(RESULT_PREFIX);
    print_string(copy_buffer);
    print_string(QUOTE_SUFFIX);
    print_newline();
    
    /* Test strcmp */
    print_string(STRCMP_TEST_PREFIX);
    result = strcmp(TEST_ABC, TEST_ABC);
    print_string(STRCMP_ABC_ABC_MSG);
    print_decimal(result);
    print_string(EQUAL_ZERO_SUFFIX);
    print_newline();

    print_string(STRCMP_B_PREFIX);
    result = strcmp(TEST_ABC, TEST_CDE);
    print_string(STRCMP_ABC_CDE_MSG);
    print_decimal(result);
    print_string(LESS_ZERO_SUFFIX);
    print_newline();
    
    print_string(STRCMP_C_PREFIX);
    result = strcmp(TEST_CDE, TEST_ABC);
    print_string(STRCMP_CDE_ABC_MSG);
    print_decimal(result);
    print_string(GREATER_ZERO_SUFFIX);
    print_newline();
    
    /* Test strncpy */
    print_string(STRNCPY_TEST_PREFIX);
    strncpy(work_buffer, TESTING_STRNCPY, 7);
    work_buffer[7] = 0; /* null terminate */
    print_string(FIRST_7_CHARS_PREFIX);
    print_string(work_buffer);
    print_string(QUOTE_SUFFIX);
    print_newline();
    
    /* Test strchr */
    print_string(STRCHR_TEST_PREFIX);
    if (strchr(test_string, 'R')) {
        print_string(FOUND_R_MSG);
    } else {
        print_string(R_NOT_FOUND_MSG);
    }
    print_newline();
    
    wait_for_key();
    
    /* === MEMORY FUNCTIONS === */
    print_string(MEMORY_FUNCS_HEADER);
    print_newline();
    
    /* Test memcpy */
    print_string(MEMCPY_TEST_PREFIX);
    memcpy(memory_buffer, MEMTEST, 7);
    memory_buffer[7] = 0;
    print_string(COPIED_PREFIX);
    print_string(memory_buffer);
    print_string(QUOTE_SUFFIX);
    print_newline();
    
    /* Test memset */
    print_string(MEMSET_TEST_PREFIX);
    memset(memory_buffer, 'X', 5);
    memory_buffer[5] = 0;
    print_string(MEMSET_RESULT_PREFIX);
    print_string(memory_buffer);
    print_string(MEMSET_EXPECTED_SUFFIX);
    print_newline();
    
    /* Test memcmp */
    print_string(MEMCMP_ABC_ABC_PREFIX);
    result = memcmp(TEST_ABC, TEST_ABC, 3);
    print_string(RESULT_EQUALS_PREFIX);
    print_decimal(result);
    print_string(EQUAL_ZERO_SUFFIX);
    print_newline();
    
    print_string(MEMCMP_ABC_DEF_PREFIX);
    result = memcmp(TEST_ABC, TEST_DEF, 3);
    print_string(RESULT_EQUALS_PREFIX);
    print_decimal(result);
    print_string(LESS_ZERO_SUFFIX);
    print_newline();
    
    print_string(MEMCMP_DEF_ABC_PREFIX);
    result = memcmp(TEST_DEF, TEST_ABC, 3);
    print_string(RESULT_EQUALS_PREFIX);
    print_decimal(result);
    print_string(GREATER_ZERO_SUFFIX);
    print_newline();
    
    wait_for_key();
    
    /* === CHARACTER FUNCTIONS === */
    print_string(CHAR_FUNCS_HEADER);
    print_newline();
    
    /* Test isalpha */
    print_string(ISALPHA_A_PREFIX);
    result = isalpha('A') ? 1 : 0;
    print_decimal(result);
    print_string(TRUE_SUFFIX);
    print_newline();
    
    print_string(ISALPHA_0_PREFIX);
    result = isalpha('0') ? 1 : 0;
    print_decimal(result);
    print_string(FALSE_SUFFIX);
    print_newline();
    
    /* Test isdigit */
    print_string(ISDIGIT_5_PREFIX);
    result = isdigit('5') ? 1 : 0;
    print_decimal(result);
    print_string(TRUE_SUFFIX);
    print_newline();
    
    print_string(ISDIGIT_X_PREFIX);
    result = isdigit('X') ? 1 : 0;
    print_decimal(result);
    print_string(FALSE_SUFFIX);
    print_newline();
    
    /* Test toupper */
    print_string(TOUPPER_A_PREFIX);
    print_char(toupper('a'));
    print_string(SHOULD_BE_A_SUFFIX);
    print_newline();
    
    /* Test tolower */
    print_string(TOLOWER_Z_PREFIX);
    print_char(tolower('Z'));
    print_string(SHOULD_BE_Z_SUFFIX);
    print_newline();
    
    wait_for_key();
    
    /* === MATH FUNCTIONS === */
    print_string(MATH_FUNCS_HEADER);
    print_newline();
    
    /* Test abs */
    print_string(ABS_TEST_PREFIX);
    positive_result = abs(negative_number);
    print_decimal(positive_result);
    print_newline();
    
    /* Test multiplication (uses runtime functions) */
    print_string(MULT_TEST_PREFIX);
    result = math_a * math_b;
    print_decimal(result);
    print_newline();
    
    /* Test division (uses runtime functions) */
    print_string(DIV_TEST_PREFIX);
    result = math_a / math_b;
    print_decimal(result);
    print_newline();
    
    /* Test modulo (uses runtime functions) */
    print_string(MOD_TEST_PREFIX);
    result = math_a % math_b;
    print_decimal(result);
    print_newline();
    
    wait_for_key();
    
    /* === CONVERSION FUNCTIONS === */
    print_string(CONVERT_FUNCS_HEADER);
    print_newline();
    
    /* Test atoi */
    print_string(ATOI_TEST_PREFIX);
    result = atoi(ATOI_TEST_VALUE);
    print_decimal(result);
    print_newline();
    
    wait_for_key();
    
    print_string(TEST_COMPLETE_HEADER);
    print_newline();
    print_string(ALL_FUNCS_TESTED_MSG);
    print_newline();
    
    return 42;
}
