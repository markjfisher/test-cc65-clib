"""
Shared harness for the cc65 bbc clib beebium integration tests.

These tests drive cc65-compiled C programs inside a fully emulated BBC Micro
(beebium) loaded from a DFS disc image, and verify their behaviour by reading
the MODE 7 screen.

Key facts that make this work (and that the earlier attempt got wrong):

* The cc65 bbc target defaults to a load/run address of $0E00, which collides
  with Acorn DFS private workspace ($0E00-$18FF). A program *RUN from a DFS disc
  there silently corrupts DFS and produces no output. build_test_discs.sh
  therefore links the test programs at $1900 (PAGE when DFS is present), and the
  disc catalogue load/exec addresses match.

* beebium's beebium.screen helpers already correct for 6845 hardware scrolling
  (they anchor reads at the CRTC screen-start), so read_mode7_screen /
  screen_contains return what is actually displayed.

* Disc operations are slow at the default 2 MHz; we set the speed multiplier to
  0.0 (run as fast as the host allows) so tests complete quickly.

* A cc65 program's _exit returns cleanly to BASIC (RTS to the language), so the
  emulator keeps running and the screen can be read after the program finishes.
"""

from __future__ import annotations

import contextlib
import os
import time
from pathlib import Path

import pytest

from beebium import Beebium
from beebium.screen import dump_screen, read_mode7_screen, screen_contains


def _getenv_path(name, default=None):
    v = os.environ.get(name) or default
    return Path(v).resolve() if v else None


def _rom(name):
    d = _getenv_path("BEEBIUM_ROM_DIR")
    if d:
        p = d / name
        if p.exists():
            return p
    return None


BEEBIUM_SERVER = _getenv_path(
    "BEEBIUM_SERVER",
    "../beebium/build-release/src/server/beebium-model-b",
)
DFS_ROM = _rom("acorn-dfs_2_26.rom")
MOS_ROM = _rom("acorn-mos_1_20.rom")
BASIC_ROM = _rom("bbc-basic_2.rom")
DISCS_DIR = (
    Path(__file__).resolve().parents[3] / "build" / "integration-testing" / "discs"
)

_REQUIRED = {
    "beebium-server": BEEBIUM_SERVER,
    "DFS ROM": DFS_ROM,
    "MOS ROM": MOS_ROM,
    "BASIC ROM": BASIC_ROM,
}
_MISSING = [n for n, p in _REQUIRED.items() if not p or not p.exists()]
SKIP_NEEDED = bool(_MISSING)
SKIP_REASON = "missing: " + ", ".join(_MISSING) if _MISSING else ""

# DFS sideways slot for the disc filing system ROM.
DFS_SLOT = 12


def disc_path(short_name: str) -> Path:
    """Path to a built test disc image (e.g. "TCONSL" -> .../TCONSL.ssd)."""
    return DISCS_DIR / f"{short_name}.ssd"


def requires_disc(short_name: str):
    """Skip marker that also checks the specific disc image was built."""
    if SKIP_NEEDED:
        return pytest.mark.skipif(True, reason=SKIP_REASON)
    return pytest.mark.skipif(
        not disc_path(short_name).exists(),
        reason=f"disc image {short_name}.ssd not built "
        f"(run tests/integration/scripts/build_test_discs.sh)",
    )


@contextlib.contextmanager
def launch(disc_short_name: str | None = None):
    """Launch beebium as a Model B with DFS + a 1770 controller.

    If ``disc_short_name`` is given, its .ssd is inserted in drive 0.
    The emulator is run as fast as the host allows.
    """
    extra = [
        "--sideways", f"{DFS_SLOT}:rom:{DFS_ROM}",
        "--fdc", "acorn-1770",
    ]
    if disc_short_name is not None:
        extra += ["--floppy", f"0:{disc_path(disc_short_name)}"]

    with Beebium.launch(
        server_filepath=str(BEEBIUM_SERVER),
        mos_filepath=str(MOS_ROM),
        basic_filepath=str(BASIC_ROM),
        extra_args=extra,
    ) as bbc:
        with contextlib.suppress(Exception):
            bbc.system.set_speed_multiplier(0.0)
        yield bbc


def boot(bbc, wait: float = 1.5):
    """Reset and let the machine reach the BASIC prompt."""
    bbc.debugger.reset()
    bbc.debugger.run()
    time.sleep(wait)


def type_line(bbc, text: str, wait: float = 0.0):
    """Type ``text`` followed by Return (the emulator must be running)."""
    bbc.keyboard.type(text + "\r")
    if wait:
        time.sleep(wait)


def pause(bbc):
    """Stop the CPU so screen/CRTC reads describe a single, settled frame."""
    bbc.debugger.stop()


def run_disc_program(bbc, short_name: str, settle: float = 3.0):
    """*RUN a program from the mounted disc, wait, then pause for reading."""
    type_line(bbc, f"*RUN {short_name}")
    time.sleep(settle)
    pause(bbc)


def screen_text(bbc) -> str:
    """Full MODE 7 screen as a diagnostic string."""
    return dump_screen(bbc)


def wait_for_text(bbc, text: str, timeout: float = 8.0, poll: float = 0.1) -> bool:
    """Poll until ``text`` appears or ``timeout`` elapses.

    Reads are taken with the CPU paused (so they are self-consistent) and the
    CPU is resumed between polls. Leaves the CPU paused on success.
    """
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        bbc.debugger.stop()
        if screen_contains(bbc, text):
            return True
        bbc.debugger.run()
        time.sleep(poll)
    bbc.debugger.stop()
    return False


def assert_screen(bbc, text: str):
    """Assert ``text`` is present on screen, dumping the screen on failure."""
    if not screen_contains(bbc, text):
        raise AssertionError(f"expected {text!r} on screen\n{dump_screen(bbc)}")


def assert_not_screen(bbc, text: str):
    """Assert ``text`` is NOT present on screen."""
    if screen_contains(bbc, text):
        raise AssertionError(f"did not expect {text!r} on screen\n{dump_screen(bbc)}")


def row_text(bbc, row: int) -> str:
    """The text of a single (scroll-corrected) MODE 7 row, right-stripped."""
    rows = read_mode7_screen(bbc)
    return rows[row].rstrip() if 0 <= row < len(rows) else ""
