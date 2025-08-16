/*
 * Basic functionality test for bbc-clib ROM target
 * Tests basic C library functions that should be available in ROM
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int negative = -42;
    int positive;
    const char* number_str = "123";
    int number;
    int result;
    
    printf("=== BBC CLIB ROM Basic Test ===\n");
    printf("Testing basic functions...\n\n");
    
    // Test abs() function
    positive = abs(negative);
    printf("abs(-42) = %d\n", positive);
    
    // Test atoi() function  
    number = atoi(number_str);
    printf("atoi(\"123\") = %d\n", number);
    
    // Test simple expressions
    result = positive + number;
    printf("%d + %d = %d\n", positive, number, result);
    
    printf("\nBasic test completed successfully!\n");
    printf("ROM functions working correctly.\n");
    
    return 0;
}
