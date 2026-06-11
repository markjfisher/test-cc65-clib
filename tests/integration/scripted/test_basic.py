"""
Integration tests for cc65-bbc C programs running under beebium.

Uses prebuilt DFS disc images. To rebuild: bash tests/integration/scripts/build_test_discs.sh
"""

from __future__ import annotations

import time
from pathlib import Path

import pytest

from beebium import Beebium
from beebium.screen import read_mode7_screen, screen_contains


BEEBIUM_SERVER = Path("/home/markf/dev/bbc/beebium/build-release/src/server/beebium-model-b")
DFS_ROM = Path("/home/markf/dev/bbc/beebium/build-release/roms/acorn-dfs_2_26.rom")
MOS_ROM = Path.home() / ".local/share/beebium/roms/acorn-mos_1_20.rom"
BASIC_ROM = Path.home() / ".local/share/beebium/roms/bbc-basic_2.rom"
DISCS_DIR = Path(__file__).resolve().parent.parent / "discs"


def _check():
    missing = [n for n, p in [
        ("beebium-server", BEEBIUM_SERVER),
        ("DFS ROM", DFS_ROM),
        ("MOS ROM", MOS_ROM),
        ("BASIC ROM", BASIC_ROM),
    ] if not p.exists()]
    if missing:
        pytest.skip(f"missing: {', '.join(missing)}")


def _launch(test_name):
    ssd = DISCS_DIR / f"T{test_name}.ssd"
    if not ssd.exists():
        pytest.fail(f"SSD not found: {ssd} — run build_test_discs.sh first")
    disc_path = str(ssd.resolve())
    dfs_path = str(DFS_ROM.resolve())
    return Beebium.launch(
        server_filepath=str(BEEBIUM_SERVER),
        mos_filepath=str(MOS_ROM),
        basic_filepath=str(BASIC_ROM),
        extra_args=[
            "--fdc", "acorn-1770",
            "--floppy", f"0:{disc_path}",
            "--sideways", f"14:rom:{dfs_path}",
        ],
    )


def _boot_and_run(bbc, cmd, boot=2.0, run=3.0):
    bbc.debugger.stop()
    bbc.debugger.reset()
    bbc.debugger.run()
    time.sleep(boot)
    bbc.debugger.stop()
    bbc.keyboard.type(f"{cmd}\n")
    bbc.debugger.run()
    time.sleep(run)
    bbc.debugger.stop()


def _text(bbc):
    return "\n".join(read_mode7_screen(bbc))


def _assert(bbc, text):
    assert screen_contains(bbc, text), (
        f"Expected {text!r} on screen\n{_text(bbc)}"
    )


# ---- Tests --------------------------------------------------------------


def test_console_io():
    """cputc, gotoxy, wherex, wherey, textcolor, bordercolor"""
    _check()
    with _launch("CONSL") as bbc:
        _boot_and_run(bbc, "*TCONSL")
        _assert(bbc, "ABC")
        _assert(bbc, "XY:10,5")
        _assert(bbc, "COLOROK")


def test_file_io():
    """fopen, fprintf, fclose, fread on DFS"""
    _check()
    with _launch("FDISK") as bbc:
        _boot_and_run(bbc, "*TFDISK")
        _assert(bbc, "FILEOK")


def test_clock():
    """clock() returns incrementing values"""
    _check()
    with _launch("CLOCK") as bbc:
        _boot_and_run(bbc, "*TCLOCK")
        _assert(bbc, "CLOCKOK")


def test_break_handler():
    """set_brk_ret + BRK + disarm_brk_ret"""
    _check()
    with _launch("BREAK") as bbc:
        _boot_and_run(bbc, "*TBREAK")
        _assert(bbc, "BRKOK")


def test_screen_ops():
    """clrscr, cputc loop, gotoxy after scroll"""
    _check()
    with _launch("SCREN") as bbc:
        _boot_and_run(bbc, "*TSCREN")
        _assert(bbc, "MODE7OK")
        for i in range(5):
            _assert(bbc, f"{i}: LINE")


def test_kbhit_cgetc():
    """kbhit, cgetc with injected keypress"""
    _check()
    with _launch("KBHIT") as bbc:
        _boot_and_run(bbc, "*TKBHIT")
        text = _text(bbc)
        print(f"\n--- Before key ---\n{text}\n------------------")

        bbc.debugger.stop()
        bbc.keyboard.type("X")
        bbc.debugger.run()
        time.sleep(1.0)
        bbc.debugger.stop()

        text = _text(bbc)
        print(f"\n--- After key ---\n{text}\n-----------------")
        _assert(bbc, "X OK")