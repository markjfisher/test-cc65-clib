;
; DEBUG-ONLY Startup code for cc65 BBC CLIB ROM testing
; This is a MINIMAL startup for ROM testing - NOT for production!
; Production apps should use the full cc65/libsrc/bbc-clib/crt0.s
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         _main                   ; Application's main function (C convention)
        .import         detect_clib_rom
        .import         clib_rom_available
        .import         clib_rom_slot
        .import         original_romsel
        .import         print_error_and_exit
        .import         rom_error_msg
        .import         OSWRCH                  ; For debug output
        .import         __HIMEM__               ; Top of memory (defined by linker)
        .importzp       c_sp                    ; C software stack pointer

        .export         __Cstart

        .include        "zeropage.inc"

.segment        "STARTUP"

__Cstart:
        ; Save stack pointer for clean exit
        tsx
        stx        save_stack

        ; Setup minimal C runtime
        ; Setup software stack pointer to top of memory
        lda        #<__HIMEM__
        sta        c_sp
        lda        #>__HIMEM__
        sta        c_sp+1

reset:
        ; Debug: Print startup indicator
        lda        #'S'
        jsr        OSWRCH

        ; Check for cc65 CLIB ROM
        jsr        detect_clib_rom

        ; Debug: Print after ROM detection
        lda        #'D'
        jsr        OSWRCH

        ; ROM must be present - exit with error if not found
        lda        clib_rom_available
        bne        rom_found

        ; Debug: Print ROM not found
        lda        #'N'
        jsr        OSWRCH

        ; ROM not found - display error and exit
        lda        #<rom_error_msg
        ldx        #>rom_error_msg
        jsr        print_error_and_exit
        ; After error message, halt completely - don't continue to rom_found
        jmp        exit_restore_stack

rom_found:
        ; Debug: Print ROM found
        lda        #'F'
        jsr        OSWRCH

        ; Debug: Print ROM slot number
        lda        #'['
        jsr        OSWRCH
        lda        clib_rom_slot
        jsr        print_hex_digit
        lda        #']'
        jsr        OSWRCH

        ; start new line after debug output
        lda        #13
        jsr        OSWRCH
        lda        #10
        jsr        OSWRCH


        ; ; Debug: Verify ROM function is accessible at expected address  
        ; ; Print first 4 bytes of _strlen at $999C (should be 85 5E 86 5F)
        ; lda        #'@'           ; Debug indicator
        ; jsr        OSWRCH

        ; ; Print 4 bytes in hex
        ; lda        $999C          ; First byte (should be $85)
        ; jsr        print_hex_byte
        ; lda        $999D          ; Second byte (should be $5E) 
        ; jsr        print_hex_byte
        ; lda        $999E          ; Third byte (should be $86)
        ; jsr        print_hex_byte
        ; lda        $999F          ; Fourth byte (should be $5F)
        ; jsr        print_hex_byte

        ; Call main function directly (minimal setup)
        jsr        _main
        ; jsr        callmain   ; not using this in debug version, it just gets argc/argv values

_exit:
        ; Invalidate ROM detection state to force fresh scan on next run
        lda        #0
        sta        clib_rom_available  ; Clear "ROM found" flag
        sta        clib_rom_slot       ; Clear slot number

        ; Restore original ROMSEL before exit
        lda        original_romsel
        sta        $FE30

exit_restore_stack:
        ; Restore original 6502 hardware stack pointer
        ldx        save_stack
        txs
        rts

; print_hex_byte - Print byte in A as two hex digits
; Input: A = byte to print
; Destroys: A, X

; print_hex_byte:
;         ; Save original value
;         pha

;         ; Print high nibble
;         lsr                     ; Shift right 4 times
;         lsr
;         lsr  
;         lsr
;         jsr     print_hex_digit

;         ; Print low nibble
;         pla                     ; Restore original
;         and     #$0F            ; Keep only low nibble
;         jsr     print_hex_digit
;         rts

; ; print_hex_digit - Print single hex digit (0-15)
; ; Input: A = digit (0-15)
; ; Destroys: A
print_hex_digit:
        cmp     #10
        bcc     print_decimal   ; 0-9
        ; A-F 
        clc
        adc     #'A'-10
        jmp     print_char
print_decimal:
        clc
        adc     #'0'
print_char:
        jsr     OSWRCH
        rts

        .bss
save_stack:     .res    1       ; Save original 6502 stack pointer
