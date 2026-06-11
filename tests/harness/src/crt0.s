; cc65 bbc target test harness - entry point and startup
;
; This harness sets up a minimal BBC MOS environment for soft65c02_unit
; to test individual cc65 bbc library functions in isolation.
;
; Test patterns:
;   1. Harness calls a test function (e.g. test_strlen) then hits brk.
;   2. The soft65c02_unit DSL loads the compiled cc65 bbc library symbols,
;      sets up memory/registers, runs until an end marker, then asserts state.
;
; The test binary and cc65 library .lbl file must be converted to .inc
; before building the harness (run create_app_inc.sh).

        .export     halt
        .export     init_harness
        .export     end_init_harness

        .import     _test_func

.ifdef APP_INC_PRESENT
        .include    "app.inc"
.endif

.segment "STARTUP"
halt:
        .byte   $db         ; STP in 65c02 emulator

.segment "CODE"
init_harness:
        ; Call the test function under test
        jsr     _test_func
end_init_harness:
        brk

; Reset vector points to halt
.segment "V_RESET"
        .word halt