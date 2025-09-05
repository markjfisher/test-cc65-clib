/*
 * Math functions test for bbc-clib ROM target
 * Tests mathematical functions available in ROM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

int main(void) {
    long big_negative = -98765L;
    char buffer[20];
    int value;
    char c;
    
    printf("=== BBC CLIB ROM Maths Test ===\n");
    printf("Testing maths functions...\n\n");
    
    // Test abs()
    printf("abs(-123) = %d\n", abs(-123));
    printf("abs(456) = %d\n", abs(456));
    
    // Test labs() (long abs)
    printf("labs(-98765L) = %ld\n", labs(big_negative));
    
    // Test atoi() and atol()
    printf("atoi(\"-42\") = %d\n", atoi("-42"));
    printf("atol(\"123456\") = %ld\n", atol("123456"));
    
    // Test itoa() and ltoa()
    itoa(789, buffer, 10);
    printf("itoa(789) = \"%s\"\n", buffer);
    
    ltoa(-54321L, buffer, 10);
    printf("ltoa(-54321L) = \"%s\"\n", buffer);
    
    // Test some conversions
    value = atoi("999");
    itoa(value * 2, buffer, 10);
    printf("atoi(\"999\") * 2 = %s\n", buffer);
    
    printf("\nMath test completed\n");
    printf("Press q to exit\n");
    c = 0;
    while ((c = cgetc()) != 'q') {
        printf("You pressed: %c\n", c);
    };
    
    return 0;
}
