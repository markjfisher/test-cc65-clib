.export _test_func
.export _test_result

.import _cputc
.import pusha

.segment "CODE"

_test_result:
.res 1

.code
_test_func:
  lda #'A'
  jsr pusha
  jsr _cputc

  lda #$0A
  jsr pusha
  jsr _cputc

  lda #'!'
  jsr pusha
  jsr _cputc

  lda #0
  jsr pusha
  jsr _cputc

  lda #1
  sta _test_result + 0

  rts