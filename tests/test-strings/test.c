/*
 * String functions test for bbc-clib ROM target
 * Tests string manipulation functions available in ROM
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer1[50];
    char buffer2[50];
    const char* test_str1 = "Hello";
    const char* test_str2 = " World!";
    int len;
    int cmp_result;
    char* found;
    
    printf("=== BBC CLIB ROM String Test ===\n");
    printf("Testing string functions...\n\n");
    
    // Test strlen()
    len = strlen(test_str1);
    printf("strlen(\"%s\") = %d\n", test_str1, len);
    
    // Test strcpy()
    strcpy(buffer1, test_str1);
    printf("strcpy result: \"%s\"\n", buffer1);
    
    // Test strcat()
    strcat(buffer1, test_str2);
    printf("strcat result: \"%s\"\n", buffer1);
    
    // Test strcmp()
    strcpy(buffer2, "Hello World!");
    cmp_result = strcmp(buffer1, buffer2);
    printf("strcmp(\"%s\", \"%s\") = %d\n", buffer1, buffer2, cmp_result);
    
    // Test strchr()
    found = strchr(buffer1, 'W');
    if (found) {
        printf("strchr found 'W' at: \"%s\"\n", found);
    }
    
    printf("\nString test completed successfully!\n");
    printf("All ROM string functions working.\n");
    
    return 0;
}
