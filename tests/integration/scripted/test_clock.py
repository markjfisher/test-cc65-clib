"""Integration test: clock() against the real BBC MOS centisecond timer.

Runs discs/test_clock.c (built as TCLOCK), which reads clock() twice with a
busy-wait between and prints CLOCKOK only if both reads are valid and the timer
advanced monotonically.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@h.requires_disc("TCLOCK")
def test_clock_advances():
    with h.launch("TCLOCK") as bbc:
        h.boot(bbc)
        h.run_disc_program(bbc, "TCLOCK", settle=4.0)

        h.assert_screen(bbc, "CLOCKOK")
        h.assert_not_screen(bbc, "CLOCKBAD")
