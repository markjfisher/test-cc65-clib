/*
 * File handling test for bbc-clib ROM target
 * Tests file operations: open, close, read, write, lseek, opendir, readdir, closedir
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <conio.h>

/* Public C ABI from the library for debug break handling */
extern unsigned char __fastcall__ set_brk_ret_debug(void);
extern void          __fastcall__ set_brk_debug_mode_only(void);
extern void          __fastcall__ disarm_brk_ret(void);

#define TEST_FILE1 "TESTFILE1"
#define TEST_FILE2 "TESTFILE2"
#define TEST_FILE3 "TESTFILE3"
#define TEST_DIR "."

void test_basic_file_operations(void);
void test_file_seeking(void);
void test_multiple_files(void);
void test_directory_operations(void);
void test_error_conditions(void);
void test_file_descriptor_limits(void);

int main(void) {
    char c;
    
    printf("=== BBC CLIB ROM File Operations Test ===\n");
    printf("Enabling debug break handling...\n");
    set_brk_debug_mode_only();
    printf("Debug break handling enabled.\n");
    printf("Testing file handling functions...\n\n");
    
    // Test basic file operations
    test_basic_file_operations();
    
    // Test file seeking
    test_file_seeking();
    
    // Test multiple files
    test_multiple_files();
    
    // Test directory operations
    test_directory_operations();
    
    // Test error conditions
    test_error_conditions();
    
    // Test file descriptor limits
    test_file_descriptor_limits();
    
    printf("\n=== All File Tests Completed ===\n");
    printf("Press q to exit\n");
    c = 0;
    while ((c = cgetc()) != 'q') {
        printf("You pressed: %c\n", c);
    };
    
    return 0;
}

void test_basic_file_operations(void) {
    int fd;
    char buffer[100];
    int bytes_written, bytes_read;
    
    printf("--- Basic File Operations ---\n");
    
    // Test file creation and writing
    printf("Creating and writing to %s...\n", TEST_FILE1);
    printf("About to call open()...\n");
    fd = open(TEST_FILE1, O_WRONLY | O_CREAT);
    printf("open() returned: %d\n", fd);
    if (fd == -1) {
        printf("ERROR: Failed to create %s (errno=%d)\n", TEST_FILE1, errno);
        return;
    }
    printf("File descriptor: %d\n", fd);
    
    printf("About to write first string...\n");
    bytes_written = write(fd, "Hello, BBC Micro!\n", 18);
    printf("Wrote %d bytes\n", bytes_written);
    
    printf("About to write second string...\n");
    bytes_written = write(fd, "This is a test file.\n", 21);
    printf("Wrote %d bytes\n", bytes_written);
    
    if (close(fd) == -1) {
        printf("ERROR: Failed to close file (errno=%d)\n", errno);
    } else {
        printf("File closed successfully\n");
    }
    
    // Test file reading
    printf("\nReading from %s...\n", TEST_FILE1);
    fd = open(TEST_FILE1, O_RDONLY);
    if (fd == -1) {
        printf("ERROR: Failed to open %s for reading (errno=%d)\n", TEST_FILE1, errno);
        return;
    }
    
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read %d bytes: %s", bytes_read, buffer);
    } else {
        printf("ERROR: Failed to read from file (errno=%d)\n", errno);
    }
    
    if (close(fd) == -1) {
        printf("ERROR: Failed to close file (errno=%d)\n", errno);
    } else {
        printf("File closed successfully\n");
    }
    
    printf("\n");
}

void test_file_seeking(void) {
    int fd;
    char buffer[50];
    off_t pos;
    int bytes_read;
    
    printf("--- File Seeking Operations ---\n");
    
    // Create a test file with known content
    printf("Creating test file for seeking...\n");
    fd = open(TEST_FILE2, O_WRONLY | O_CREAT);
    if (fd == -1) {
        printf("ERROR: Failed to create %s (errno=%d)\n", TEST_FILE2, errno);
        return;
    }
    
    write(fd, "0123456789ABCDEFGHIJ", 20);
    close(fd);
    
    // Test seeking from beginning
    printf("Testing SEEK_SET...\n");
    fd = open(TEST_FILE2, O_RDONLY);
    if (fd == -1) {
        printf("ERROR: Failed to open %s (errno=%d)\n", TEST_FILE2, errno);
        return;
    }
    
    pos = lseek(fd, 5, SEEK_SET);
    printf("Seeked to position %ld\n", (long)pos);
    
    bytes_read = read(fd, buffer, 5);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read from position 5: %s\n", buffer);
    }
    
    // Test seeking from current position
    printf("Testing SEEK_CUR...\n");
    pos = lseek(fd, 3, SEEK_CUR);
    printf("Seeked to position %ld\n", (long)pos);
    
    bytes_read = read(fd, buffer, 5);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read from current+3: %s\n", buffer);
    }
    
    // Test seeking from end
    printf("Testing SEEK_END...\n");
    pos = lseek(fd, -5, SEEK_END);
    printf("Seeked to position %ld (from end)\n", (long)pos);
    
    bytes_read = read(fd, buffer, 5);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read from end-5: %s\n", buffer);
    }
    
    close(fd);
    printf("\n");
}

void test_multiple_files(void) {
    int fd1, fd2, fd3;
    char buffer[50];
    int bytes_written, bytes_read;
    
    printf("--- Multiple File Operations ---\n");
    
    // Open multiple files simultaneously
    printf("Opening multiple files...\n");
    fd1 = open(TEST_FILE1, O_RDONLY);
    fd2 = open(TEST_FILE2, O_RDONLY);
    fd3 = open(TEST_FILE3, O_WRONLY | O_CREAT);
    
    printf("File descriptors: %d, %d, %d\n", fd1, fd2, fd3);
    
    if (fd1 != -1 && fd2 != -1 && fd3 != -1) {
        // Read from first file
        bytes_read = read(fd1, buffer, 10);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Read from fd1: %s\n", buffer);
        }
        
        // Read from second file
        bytes_read = read(fd2, buffer, 10);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Read from fd2: %s\n", buffer);
        }
        
        // Write to third file
        bytes_written = write(fd3, "Multi-file test\n", 16);
        printf("Wrote %d bytes to fd3\n", bytes_written);
        
        // Close all files
        close(fd1);
        close(fd2);
        close(fd3);
        printf("All files closed\n");
    } else {
        printf("ERROR: Failed to open some files\n");
    }
    
    printf("\n");
}

void test_directory_operations(void) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    
    printf("--- Directory Operations ---\n");
    
    printf("Opening directory %s...\n", TEST_DIR);
    dir = opendir(TEST_DIR);
    if (dir == NULL) {
        printf("ERROR: Failed to open directory (errno=%d)\n", errno);
        return;
    }
    
    printf("Reading directory entries...\n");
    while ((entry = readdir(dir)) != NULL && count < 10) {
        printf("Entry %d: %s\n", count, entry->d_name);
        count++;
    }
    
    if (closedir(dir) == 0) {
        printf("Directory closed successfully\n");
    } else {
        printf("ERROR: Failed to close directory (errno=%d)\n", errno);
    }
    
    printf("Found %d directory entries\n", count);
    printf("\n");
}

void test_error_conditions(void) {
    int fd;
    char buffer[10];
    int result;
    
    printf("--- Error Condition Tests ---\n");
    
    // Test opening non-existent file
    printf("Testing open of non-existent file...\n");
    fd = open("NONEXISTENT", O_RDONLY);
    if (fd == -1) {
        printf("Correctly failed to open non-existent file (errno=%d)\n", errno);
    } else {
        printf("ERROR: Should have failed to open non-existent file\n");
        close(fd);
    }
    
    // Test reading from invalid file descriptor
    printf("Testing read from invalid fd...\n");
    result = read(999, buffer, sizeof(buffer));
    if (result == -1) {
        printf("Correctly failed to read from invalid fd (errno=%d)\n", errno);
    } else {
        printf("ERROR: Should have failed to read from invalid fd\n");
    }
    
    // Test writing to read-only file
    printf("Testing write to read-only file...\n");
    fd = open(TEST_FILE1, O_RDONLY);
    if (fd != -1) {
        result = write(fd, "test", 4);
        if (result == -1) {
            printf("Correctly failed to write to read-only file (errno=%d)\n", errno);
        } else {
            printf("ERROR: Should have failed to write to read-only file\n");
        }
        close(fd);
    }
    
    // Test seeking on invalid file descriptor
    printf("Testing seek on invalid fd...\n");
    result = lseek(999, 0, SEEK_SET);
    if (result == -1) {
        printf("Correctly failed to seek on invalid fd (errno=%d)\n", errno);
    } else {
        printf("ERROR: Should have failed to seek on invalid fd\n");
    }
    
    printf("\n");
}

void test_file_descriptor_limits(void) {
    int fds[20];
    int i, success_count = 0;
    
    printf("--- File Descriptor Limit Tests ---\n");
    
    printf("Testing file descriptor allocation...\n");
    
    // Try to open many files to test fd limits
    for (i = 0; i < 20; i++) {
        fds[i] = open(TEST_FILE3, O_RDONLY);
        if (fds[i] != -1) {
            success_count++;
            printf("Opened fd %d: %d\n", i, fds[i]);
        } else {
            printf("Failed to open fd %d (errno=%d)\n", i, errno);
            break;
        }
    }
    
    printf("Successfully opened %d files\n", success_count);
    
    // Close all opened files
    for (i = 0; i < success_count; i++) {
        if (close(fds[i]) == -1) {
            printf("ERROR: Failed to close fd %d\n", fds[i]);
        }
    }
    
    printf("Closed all files\n");
    printf("\n");
}
