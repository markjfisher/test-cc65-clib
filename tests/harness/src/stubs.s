; Unimplemented MOS entry points — all land here via brk so the test
; harness halts on unexpected OS calls.

        .export h_unknown_entry

h_unknown_entry:
        brk