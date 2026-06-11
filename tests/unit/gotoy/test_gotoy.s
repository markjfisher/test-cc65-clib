.export _test_func
.export _test_result

.import _gotoy
.import _gotoxy
.import _wherey
.import pusha

.segment "CODE"

_test_result:
.res 3

.code
_test_func:
  lda #5
  jsr pusha
  lda #10
  jsr pusha
  jsr _gotoxy
  jsr _wherey
  sta _test_result + 0
  lda #7
  jsr _gotoy
  jsr _wherey
  sta _test_result + 1
  lda #1
  sta _test_result + 2
  rts