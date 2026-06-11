.export _test_func
.export _test_result

.import _set_brk_ret_debug
.import _set_brk_debug_mode_only
.import _disarm_brk_ret

.segment "CODE"

_test_result:
.res 4

.code
_test_func:
  jsr _set_brk_debug_mode_only
  sta _test_result + 0
  jsr _set_brk_ret_debug
  sta _test_result + 1
  jsr _disarm_brk_ret
  lda #1
  sta _test_result + 2
  rts