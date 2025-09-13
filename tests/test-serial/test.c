/*
* Test serial to Fujinet
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* BBC OS functions for simple output */
void OSWRCH(unsigned char c);

/* BBC OS read character - direct ROM call at FFE0 */
char (*OSRDCH)(void) = (char (*)(void))0xFFE0;

/* OSBYTE function declarations */
void osbyte(unsigned char op, unsigned char r1, unsigned char r2);

/* Assembly function declarations */
int read_char_with_timeout(unsigned char *ch, unsigned int timeout_centiseconds);
int check_rs423_buffer(void);
int read_rs423_char(unsigned char *ch);

/* OSBYTE constants for serial configuration */
#define osbyte_SERIAL_RECEIVE_RATE    0x07
#define osbyte_SERIAL_TRANSMIT_RATE   0x08
#define osbyte_OUTPUT_STREAMS         0x03
#define osbyte_INPUT_STREAM           0x02
#define osbyte_IN_KEY                 0x81
#define osbyte_FLUSH_BUFFERS          0x15
#define osbyte_READ_ADC               0x80
#define osbyte_REMOVE_CHAR            0x91

/* Baud rate 7 = 9600 baud */
#define BAUD_9600                     7

/* Output stream values */
#define OUTPUT_SCREEN_ONLY            0
#define OUTPUT_SERIAL_ONLY            3

/* Input stream values */
#define INPUT_KEYBOARD_ONLY           0
#define INPUT_SERIAL_ONLY             1

/* Buffer flush values */
#define FLUSH_KEYBOARD_BUFFER         0
#define FLUSH_SERIAL_INPUT_BUFFER     1

/* RS423 buffer values */
#define RS423_INPUT_BUFFER            254  /* ADVAL(-2) */
#define RS423_GET_CHAR                1    /* X=1 for RS423 in OSBYTE 145 */

/* FujiNet device ID */
#define THE_FUJI                      0x70

#define GET_SSID_LEN                  100

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
    print_char(13);  /* CR - Carriage Return (go to beginning of next line) */
    print_char(10);  /* LF - Line Feed (go to beginning of next line) */
}


void wait_for_key(void) {
    print_string("Press any key to continue...");
    print_newline();
    OSRDCH();  /* Direct BBC OS call */
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

/* Serial port configuration functions */
void setup_serial_ports(void) {
    /* *fx 7,7 - Set RX to 9600 baud */
    osbyte(osbyte_SERIAL_RECEIVE_RATE, BAUD_9600, 0);
    
    /* *fx 8,7 - Set TX to 9600 baud */
    osbyte(osbyte_SERIAL_TRANSMIT_RATE, BAUD_9600, 0);
    
    /* *fx 3,3 - Output data to serial port (not screen) */
    osbyte(osbyte_OUTPUT_STREAMS, OUTPUT_SERIAL_ONLY, 0);
    
    /* *fx 2,1 - Input from serial port (not keyboard) */
    osbyte(osbyte_INPUT_STREAM, INPUT_SERIAL_ONLY, 0);
    
    /* *fx 21,0 - Flush keyboard buffer */
    osbyte(osbyte_FLUSH_BUFFERS, FLUSH_KEYBOARD_BUFFER, 0);
    
    /* *fx 21,1 - Flush RS423 serial input buffer */
    osbyte(osbyte_FLUSH_BUFFERS, FLUSH_SERIAL_INPUT_BUFFER, 0);

}    

void reset_serial_to_screen(void) {
    /* *fx 3,0 - Reset screen output only */
    osbyte(osbyte_OUTPUT_STREAMS, OUTPUT_SCREEN_ONLY, 0);
    
    /* *fx 2,0 - Reset keyboard input only */
    osbyte(osbyte_INPUT_STREAM, INPUT_KEYBOARD_ONLY, 0);
}

/* FujiNet checksum calculation */
unsigned char rs232_checksum(unsigned char *buf, unsigned short len) {
    unsigned int chk = 0;
    int i;
    
    for (i = 0; i < len; i++) {
        chk = ((chk + buf[i]) >> 8) + ((chk + buf[i]) & 0xff);
    }
    
    return (unsigned char)chk;
}

/* Send data to FujiNet with proper protocol */
void send_data(unsigned char command, unsigned char arg1, unsigned char arg2, unsigned char arg3, unsigned char arg4) {
    unsigned char packet[7];
    unsigned char checksum;
    
    /* Build packet: device_id, command, arg1, arg2, arg3, arg4, checksum */
    packet[0] = THE_FUJI;        /* Device ID */
    packet[1] = command;         /* Command */
    packet[2] = arg1;            /* Arg 1 */
    packet[3] = arg2;            /* Arg 2 */
    packet[4] = arg3;            /* Arg 3 */
    packet[5] = arg4;            /* Arg 4 */
    
    /* Calculate checksum for first 6 bytes */
    checksum = rs232_checksum(packet, 6);
    packet[6] = checksum;
    
    /* Send the packet */
    {
        int i;
        for (i = 0; i < 7; i++) {
            OSWRCH(packet[i]);
        }
    }

}

/* Helper function to check if character is printable */
int is_printable(unsigned char c) {
    return (c >= 32 && c <= 126);
}

/* Hex dump function for received data */
void hex_dump(unsigned char *data, int len) {
    int i, j;
    unsigned char c;
    
    for (i = 0; i < len; i += 8) {
        /* Print address */
        print_string("    ");
        print_hex_byte((i >> 8) & 0xFF);
        print_hex_byte(i & 0xFF);
        print_string("  ");
        
        /* Print hex bytes */
        for (j = 0; j < 8 && (i + j) < len; j++) {
            print_hex_byte(data[i + j]);
            print_char(' ');
        }
        
        /* Pad with spaces if less than 8 bytes */
        for (j = 8 - j; j > 0; j--) {
            print_string("   ");
        }
        
        print_string(" |");
        
        /* Print ASCII representation */
        for (j = 0; j < 8 && (i + j) < len; j++) {
            c = data[i + j];
            if (is_printable(c)) {
                print_char(c);
            } else {
                print_char('.');
            }
        }
        
        print_string("|");
        print_newline();
    }
}

/* FujiNet reset command */
void fn_reset(void) {
    /* Setup serial ports for 9600 baud */
    setup_serial_ports();

    /* Send reset command with 4 zero arguments */
    send_data(0xFF, 0x00, 0x00, 0x00, 0x00);

    /* Reset everything back to screen/keyboard */
    reset_serial_to_screen();
}

/* Read data from RS423 buffer with waiting */
int read_serial_data(unsigned char *buffer, int length) {
    int i;
    int bytes_received = 0;
    int wait_count = 0;
    const int max_wait = 10000; /* Maximum wait cycles per byte */
    
    for (i = 0; i < length; i++) {
        unsigned char ch = 0;
        int result = 0;
        wait_count = 0;
        
        /* Wait for data to be available in RS423 buffer */
        do {
            if (check_rs423_buffer()) {
                result = read_rs423_char(&ch);
                break;
            }
            wait_count++;
        } while (wait_count < max_wait);
        
        if (wait_count >= max_wait) {
            /* Timeout waiting for data - fill remaining with zeros and break */
            while (i < length) {
                buffer[i++] = 0;
            }
            break;
        }
        
        if (result == 1) {
            buffer[i] = ch;
            bytes_received++;
        } else {
            /* No data available - fill remaining with zeros and break */
            while (i < length) {
                buffer[i++] = 0;
            }
            break;
        }
    }
    
    return bytes_received;
}

/* FujiNet get SSID command */
void fn_get_ssid(void) {
    unsigned char response[GET_SSID_LEN];
    unsigned char payload[GET_SSID_LEN - 3];
    int i;
    int bytes_received = 0;
    int protocol_valid = 0;
    int checksum_valid = 0;
    unsigned char expected_checksum = 0;
    unsigned char received_checksum = 0;
    
    print_string("fn_get_ssid: Starting...");
    print_newline();
    
    /* Setup serial ports for 9600 baud (both input and output) */
    setup_serial_ports();
    
    /* Send get SSID command with 4 zero arguments */
    send_data(0xFE, 0x00, 0x00, 0x00, 0x00);
    
    /* Read GET_SSID_LEN bytes from RS423 buffer */
    bytes_received = read_serial_data(response, GET_SSID_LEN);
    
    /* Reset everything back to screen/keyboard */
    reset_serial_to_screen();
    
    /* Validate protocol: ACK + Completed + 97 payload + checksum */
    if (bytes_received >= GET_SSID_LEN) {
        /* Check for ACK ('A') and Completed ('C') */
        if (response[0] == 'A' && response[1] == 'C') {
            protocol_valid = 1;
            
            /* Extract payload (bytes 2-98) */
            for (i = 0; i < (GET_SSID_LEN - 3); i++) {
                payload[i] = response[i + 2];
            }
            
            /* Calculate expected checksum for payload */
            expected_checksum = rs232_checksum(payload, GET_SSID_LEN - 3);
            received_checksum = response[GET_SSID_LEN - 1];
            
            /* Validate checksum */
            if (expected_checksum == received_checksum) {
                checksum_valid = 1;
            }
        }
    }
    
    /* Display protocol validation results */
    print_string("Protocol valid: ");
    print_decimal(protocol_valid);
    print_newline();
    
    print_string("Checksum valid: ");
    print_decimal(checksum_valid);
    print_newline();
    
    print_string("Expected checksum: ");
    print_hex_byte(expected_checksum);
    print_newline();
    
    print_string("Received checksum: ");
    print_hex_byte(received_checksum);
    print_newline();
    
    /* Display the received data */
    print_string("SSID Response (100 bytes):");
    print_newline();
    hex_dump(response, GET_SSID_LEN);
    
    /* Display payload if valid */
    if (protocol_valid && checksum_valid) {
        print_string("Completed successfully");
        print_newline();
    }
}

int main(void) {
    print_string("FujiNet Serial Test Starting...");
    print_newline();
    
    /* Test reset command */
    // print_string("\n=== Testing Reset Command ===\n");
    // fn_reset();
    
    /* Test get SSID command */
    print_string("=== Testing Get SSID Command ===");
    print_newline();
    fn_get_ssid();
    
    print_string("Serial Test Complete!");
    print_newline();
    
    return 42;
}
