.export _test_func
.export _test_result

.import preservezp
.import restorezp

.segment "CODE"

_test_result:
.res 4

.code
_test_func:
  jsr preservezp
  lda #1
  sta _test_result + 0
  jsr restorezp
  lda #1
  sta _test_result + 1
  rts