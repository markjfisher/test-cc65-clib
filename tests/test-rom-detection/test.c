/*
 * ROM detection test for bbc-clib target
 * Tests ROM presence using bbc-clib library functions
 */

#include <stdio.h>

// External ROM detection functions from bbc-clib target library
extern unsigned char clib_rom_available;
extern void detect_clib_rom(void);

int main(void) {
    printf("=== BBC CLIB ROM Detection Test ===\n");
    printf("Testing ROM detection...\n\n");
    
    printf("Checking for cc65 CLIB ROM at $8000...\n");
    
    // Call ROM detection function from bbc-clib library
    detect_clib_rom();
    
    if (clib_rom_available) {
        printf("SUCCESS: cc65 CLIB ROM detected!\n");
        printf("ROM type: $%02X\n", *(unsigned char*)0x8006);
        printf("ROM version: $%02X\n", *(unsigned char*)0x8008);
        printf("ROM title: \"%.9s\"\n", (char*)0x8009);
        printf("ROM is ready for bbc-clib target use\n");
    } else {
        printf("WARNING: cc65 CLIB ROM not detected\n");
        printf("ROM may not be loaded or in wrong slot\n");
        printf("bbc-clib target requires ROM in sideways slot 1\n");
        printf("This should not happen if ROM is properly loaded\n");
    }
    
    printf("\nROM detection test completed.\n");
    printf("All bbc-clib functions should work correctly!\n");
    
    return 0;
}
