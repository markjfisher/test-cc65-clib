"""Integration test: cc65 bbc console output (cputc/cputs/gotoxy/textcolor).

Runs discs/test_console.c (built as TCONSL) and checks that characters reach
the MODE 7 screen via the real MOS OSWRCH, and that gotoxy() positions output
on the requested row.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@h.requires_disc("TCONSL")
def test_console_output():
    with h.launch("TCONSL") as bbc:
        h.boot(bbc)
        h.run_disc_program(bbc, "TCONSL")

        # cputc('A'),('B'),('C') from the home position. (cc65 clrscr() emits a
        # trailing newline after CLS, so the home row is row 1.)
        assert h.row_text(bbc, 1) == "ABC", h.screen_text(bbc)
        # gotoxy(0, 5) then cputs("POSITION")
        assert h.row_text(bbc, 5) == "POSITION", h.screen_text(bbc)
        # textcolor(1) then gotoxy(0, 6) then cputs("COLOUROK")
        assert h.row_text(bbc, 6) == "COLOUROK", h.screen_text(bbc)
