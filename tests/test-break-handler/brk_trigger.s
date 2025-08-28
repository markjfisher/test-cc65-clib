; brk_trigger.s — tiny helpers to raise BRK with chosen error byte
; Exported with leading underscores for C.

        .export _cause_brk_non_esc
        .export _cause_brk_esc

        .code

; void cause_brk_non_esc(void);
_cause_brk_non_esc:
        brk
        .byte $02          ; non-ESC => handled by our BRK handler (guarded path)
        rts                ; not reached

; void cause_brk_esc(void);
_cause_brk_esc:
        brk
        .byte $1B          ; ESC => pass-through path (may exit/chain)
        rts                ; not reached
