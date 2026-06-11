.export _test_func
.export _test_result

.import print0

.rodata
hello_str:
  .byte "Hello print0!", 0

.segment "CODE"

_test_result:
.res 2

.code
_test_func:
  lda #<hello_str
  sta $F2
  lda #>hello_str
  sta $F3

  jsr print0

  lda #1
  sta _test_result + 0
  sta _test_result + 1

  rts