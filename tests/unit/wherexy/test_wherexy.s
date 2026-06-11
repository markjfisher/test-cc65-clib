.export _test_func
.export _test_result

.import _wherex
.import _wherey
.import _gotoxy
.import pusha

.segment "CODE"

_test_result:
.res 4

.code
_test_func:
  jsr _wherex
  sta _test_result + 0
  jsr _wherey
  sta _test_result + 1
  lda #5
  jsr pusha
  lda #10
  jsr pusha
  jsr _gotoxy
  jsr _wherex
  sta _test_result + 2
  jsr _wherey
  sta _test_result + 3
  rts