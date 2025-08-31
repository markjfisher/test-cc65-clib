; brk_trigger.s — tiny helpers to raise BRK with chosen error byte
; Exported with leading underscores for C.

        .export _cause_brk_non_esc
        .export _cause_brk_esc
        .export _irq_handler

; MOS entry point for IRQ vector, which is hit when BRK executes
; https://github.com/raybellis/mos120/blob/master/src/dc1c.s

_irq_handler := $DC1C

        .code

; void cause_brk_non_esc(void);
_cause_brk_non_esc:
        brk
        .byte $02          ; non-ESC => handled by our BRK handler (guarded path)

        ; realign for debugger only, this is not needed for the code, just for the debugger view
        nop
        nop

; void cause_brk_esc(void);
_cause_brk_esc:
        brk
        .byte $1B          ; ESC => pass-through path (may exit/chain)

        ; realign for debugger only, this is not needed for the code, just for the debugger view
        nop
        nop
