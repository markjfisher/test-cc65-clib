.export _test_func
.export _test_result

.import disable_cursor_edit
.import restore_cursor_edit

.segment "CODE"

_test_result:
.res    4

.code
_test_func:
  ; Disable cursor edit uses OSBYTE #$A8 to set interpretation arrows
  ; OSBYTE stub handles this. Just verify both functions complete.
  jsr     disable_cursor_edit
  lda     #1
  sta     _test_result + 0

  jsr     restore_cursor_edit
  lda     #1
  sta     _test_result + 1

  rts