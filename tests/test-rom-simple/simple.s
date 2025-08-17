; Minimal ROM test in pure assembly
; Tests that ROM functions can be called

.export _main

.import _strlen, _abs, _strcpy
.import OSWRCH

; trying to force the startup code to be included
.forceimport    __STARTUP__

.code

_main:
    ; go to new line
    lda #13            ; CR
    jsr OSWRCH
    lda #10            ; LF
    jsr OSWRCH

    ; Print startup message
    ldx #0
:   lda startup_msg,x
    beq test_strlen
    jsr OSWRCH
    inx
    bne :-

test_strlen:
    ; Test strlen("Hello") 
    lda #<test_string
    ldx #>test_string  
    jsr _strlen
    ; Result in A/X, A=5, X=0
    
    ; Print length result  
    clc
    adc #'0'           ; Convert to ASCII digit
    jsr OSWRCH
    
    lda #13            ; CR
    jsr OSWRCH
    lda #10            ; LF
    jsr OSWRCH

test_abs:
    ; Test abs(-69)
    lda #<(-69)        ; Low byte of -69
    ldx #>(-69)        ; High byte of -69  
    jsr _abs
    ; Result should be 69
    
    ; Convert to decimal and print (simple case)
    ; 69 = 6*10 + 9
    ldy #0
:   cmp #10
    bcc print_digit
    sbc #10            ; Subtract 10 (carry already clear from cmp)
    iny               ; Count tens
    bne :-

print_digit:
    ; At this point: Y=tens digit, A=ones digit
    ; Save ones digit
    pha                 ; Save ones digit on stack
    
    ; Print tens digit
    tya                 ; Get tens digit
    clc
    adc #'0'           ; Convert to ASCII
    jsr OSWRCH         ; Print tens digit
    
    ; Print ones digit  
    pla                 ; Restore ones digit
    clc
    adc #'0'           ; Convert to ASCII
    jsr OSWRCH         ; Print ones digit
    
    lda #13            ; CR
    jsr OSWRCH
    lda #10            ; LF  
    jsr OSWRCH

success:
    ldx #0
:   lda success_msg,x
    beq done
    jsr OSWRCH
    inx
    bne :-

done:
    rts                ; Return to OS

.rodata

startup_msg:
    .byte "ROM Test: ", 0

test_string:
    .byte "Hello", 0

success_msg:
    .byte "ROM OK!", 13, 10, 0
