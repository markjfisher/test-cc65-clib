.export _test_func
.export _test_result

.import _gotox
.import _gotoy
.import pusha

.segment "CODE"

_test_result:
.res 3

.code
_test_func:
  lda #10
  jsr pusha
  jsr _gotox

  lda #5
  jsr pusha
  jsr _gotoy

  lda #1
  sta _test_result + 0
  sta _test_result + 1
  sta _test_result + 2

  rts