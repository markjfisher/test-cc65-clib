"""Integration test: cursor positioning and line output (gotoxy + scrolling).

Runs discs/test_screen.c (built as TSCREN): prints five numbered lines from
home, then jumps to row 10 with gotoxy() and prints a marker.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@h.requires_disc("TSCREN")
def test_screen_lines_and_gotoxy():
    with h.launch("TSCREN") as bbc:
        h.boot(bbc)
        h.run_disc_program(bbc, "TSCREN")

        # The five numbered lines printed from the home position. (cc65 clrscr()
        # emits CLS *and* a trailing newline, so the home block starts at row 1.)
        for i in range(5):
            assert h.row_text(bbc, i + 1) == f"{i}: LINE", h.screen_text(bbc)

        # gotoxy(0, 10) positions absolutely, independent of the home block.
        assert h.row_text(bbc, 10) == "MODE7OK", h.screen_text(bbc)
