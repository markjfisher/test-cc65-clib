.export _test_func
.export _test_result

.import _osfind

.segment "CODE"

_test_result:
.res 3

.code
_test_func:
  lda #1
  sta _test_result + 0
  rts