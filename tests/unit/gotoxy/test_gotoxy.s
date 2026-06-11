.export _test_func
.export _test_result

.import _gotox
.import _gotoy
.import _wherex
.import _wherey
.import pusha

.segment "CODE"

_test_result:
.res 5

.code
_test_func:
  lda #0
  jsr pusha
  jsr _gotox

  lda #0
  jsr pusha
  jsr _gotoy

  jsr _wherex
  sta _test_result + 0

  jsr _wherey
  sta _test_result + 1

  lda #20
  jsr pusha
  jsr _gotox

  lda #10
  jsr pusha
  jsr _gotoy

  jsr _wherex
  sta _test_result + 2

  jsr _wherey
  sta _test_result + 3

  lda #1
  sta _test_result + 4

  rts