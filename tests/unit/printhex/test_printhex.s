.export _test_func
.export _test_result

.import printhex
.import printstr

.rodata
str_hello:
.byte "HELLO", 0

.segment "CODE"

_test_result:
.res 3

.code
_test_func:
  lda str_hello
  jsr printhex
  lda #1
  sta _test_result + 0
  rts