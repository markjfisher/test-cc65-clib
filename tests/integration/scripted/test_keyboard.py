"""Integration test: blocking keyboard input via cgetc().

Runs discs/test_kbhit.c (built as TKBHIT): it prints PRESS, blocks in cgetc(),
and echoes the key it read. The test injects 'A' and checks "GOT A" appears.
"""

from __future__ import annotations

import time

import pytest

import clib_harness as h


@h.requires_disc("TKBHIT")
def test_cgetc_reads_key():
    with h.launch("TKBHIT") as bbc:
        h.boot(bbc)

        h.type_line(bbc, "*RUN TKBHIT")
        # Wait for the program to reach its cgetc() prompt.
        assert h.wait_for_text(bbc, "PRESS", timeout=6.0), h.screen_text(bbc)

        # Resume and inject the keypress cgetc() is waiting for.
        bbc.debugger.run()
        time.sleep(0.5)
        bbc.keyboard.type("A")
        time.sleep(1.5)
        bbc.debugger.stop()

        h.assert_screen(bbc, "GOT A")
