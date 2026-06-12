"""Integration test: directory listing via opendir()/readdir()/closedir().

Runs discs/test_dir.c (built as TDIR) on a disc that also holds data files
ALPHA and BETA. The listing (driven by OSGBPB against the real DFS catalogue)
must include the program and both data files.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@h.requires_disc("TDIR")
def test_directory_listing():
    with h.launch("TDIR") as bbc:
        h.boot(bbc)
        h.run_disc_program(bbc, "TDIR")

        h.assert_screen(bbc, "TDIR")
        h.assert_screen(bbc, "ALPHA")
        h.assert_screen(bbc, "BETA")
        h.assert_screen(bbc, "END")
        h.assert_not_screen(bbc, "NODIR")
