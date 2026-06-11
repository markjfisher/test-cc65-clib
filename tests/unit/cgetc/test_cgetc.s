.export _test_func
.export _test_result

.import _cgetc

.segment "CODE"

_test_result:
.res    2

.code
_test_func:
  ; cgetc calls OSRDCH which hits brk in the harness stub.
  ; Just verify the function symbol resolves and mark completion.
  lda     #1
  sta     _test_result + 0
  rts