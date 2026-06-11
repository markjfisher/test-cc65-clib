; Test source for harness smoke test.
; Loaded as an extra src_file in the YAML.
; Calls _test_func which returns 42 in A/X, then stores A to TEST_RESULT.

        .export _test_func
        .export test_result

        .segment "CODE"

test_result:
        .byte   0

_test_func:
        lda     #42
        sta     test_result
        rts