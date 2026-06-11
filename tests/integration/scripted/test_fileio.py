"""Integration test: stdio file I/O (fopen/fwrite/fclose/fread) on real DFS.

Runs discs/test_fileio.c (built as TFDISK): writes "Hello World" to a DFS file
and reads it back, expecting FILEOK.

KNOWN FAILURE (xfail): on the current cc65 bbc target this hangs at the first
OSBGET/OSBPUT. The same emulated setup performs DFS I/O correctly from BASIC
(OPENOUT/PRINT#/BGET#), so the fault is in the cc65 bbc read()/write() path
rather than DFS or the emulator. When the library is fixed, remove the xfail
mark and this test should pass.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@h.requires_disc("TFDISK")
@pytest.mark.xfail(
    reason="cc65 bbc read()/write() hang on the first OSBGET/OSBPUT; "
    "BASIC disc I/O works in the same setup (library bug, not DFS/emulator)",
    strict=False,
)
def test_file_roundtrip():
    with h.launch("TFDISK") as bbc:
        h.boot(bbc)
        h.run_disc_program(bbc, "TFDISK", settle=6.0)

        h.assert_screen(bbc, "FILEOK")
        h.assert_not_screen(bbc, "FILEBAD")
