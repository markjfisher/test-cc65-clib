"""pytest fixtures for the dual-mode (bbc / bbc-clib) integration tests.

Any test that takes a ``mode`` parameter is automatically run once per cc65
target. bbc-clib runs are skipped if the CLIB ROM has not been built.
"""

from __future__ import annotations

import pytest

import clib_harness as h


@pytest.fixture(params=h.MODES)
def mode(request):
    """The cc65 target under test: 'bbc' (RAM) or 'bbc-clib' (ROM-split)."""
    m = request.param
    if h.SKIP_NEEDED:
        pytest.skip(h.SKIP_REASON)
    if m == "bbc-clib" and not h.CLIB_ROM.exists():
        pytest.skip("roms/clib.rom not built (run make -C build-rom)")
    return m
