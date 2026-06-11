/*
 * test_fileio.c — tests file I/O: fopen, fprintf, fclose, fread
 */
#include <stdio.h>
#include <string.h>
#include <conio.h>

int main(void) {
    FILE *fp;
    char buf[32];
    int n;

    cputs("\r\n");

    fp = fopen("TSTFILE", "w");
    if (fp == NULL) {
        cputs("FOPENFAIL\r\n");
        return 1;
    }
    fprintf(fp, "Hello World");
    fclose(fp);

    fp = fopen("TSTFILE", "r");
    if (fp == NULL) {
        cputs("FOPENFAIL2\r\n");
        return 1;
    }
    n = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[n] = '\0';
    fclose(fp);

    if (strcmp(buf, "Hello World") == 0) {
        cputs("FILEOK\r\n");
    } else {
        cputs("FILEBAD\r\n");
    }

    return 0;
}