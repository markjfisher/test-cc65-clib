.export _test_func
.export _test_result

.import _chline
.import _cvline

.segment "CODE"

_test_result:
.res 3

.code
_test_func:
  lda #15
  jsr _chline

  lda #8
  jsr _cvline

  lda #1
  sta _test_result + 0
  sta _test_result + 1
  sta _test_result + 2

  rts