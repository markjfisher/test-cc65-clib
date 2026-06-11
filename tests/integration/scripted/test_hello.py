"""
Base integration test for cc65 bbc target.

Compiles hello.c for the bbc target, creates a DFS disc image, and
loads it in the beebium emulator. Verifies the program output appears.
"""

from __future__ import annotations

import subprocess
import time
from pathlib import Path

import pytest

from beebium import Beebium
from beebium.screen import read_mode7_screen, screen_contains


PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent.parent
CC65_CLI = PROJECT_ROOT.parent / "cc65" / "bin" / "cl65"
DISCS_DIR = PROJECT_ROOT / "tests" / "integration" / "discs"
TESTS_SSD = DISCS_DIR / "TESTS.ssd"
HELLO_C = DISCS_DIR / "hello.c"
HELLO_BIN = DISCS_DIR / "HELLO"
DISCS_JSON = DISCS_DIR / "discs.json"
DFSTOOL = Path.home() / ".cargo" / "bin" / "dfstool"
BEEBIUM_SERVER = Path("/home/markf/dev/bbc/beebium/build-release/src/server/beebium-model-b")
DFS_ROM = Path("/home/markf/dev/bbc/beebium/build-release/roms/acorn-dfs_2_26.rom")
MOS_ROM = Path.home() / ".local/share/beebium/roms/acorn-mos_1_20.rom"
BASIC_ROM = Path.home() / ".local/share/beebium/roms/bbc-basic_2.rom"


def _build_hello() -> None:
    """Compile hello.c and rebuild the disc image."""
    subprocess.run(
        [str(CC65_CLI), "-t", "bbc", "-o", str(HELLO_BIN), str(HELLO_C)],
        check=True, capture_output=True,
    )
    subprocess.run(
        [str(DFSTOOL), "make", "--output", str(TESTS_SSD), "--overwrite",
         str(DISCS_JSON)],
        check=True, capture_output=True,
    )


def test_c_program_prints_hello():
    """
    Compile hello.c with cc65 -t bbc, create DFS disc image, load in
    beebium with DFS, and verify 'HELLO' appears on the MODE 7 screen.
    """
    required = {
        "MOS ROM": MOS_ROM,
        "BASIC ROM": BASIC_ROM,
        "dfstool": DFSTOOL,
        "cc65": CC65_CLI,
    }
    missing = [name for name, p in required.items() if not p.exists()]
    if missing:
        pytest.skip(f"beebium not available — missing: {', '.join(missing)}")

    _build_hello()

    disc_path = str(TESTS_SSD.resolve())
    dfs_rom_path = str(DFS_ROM.resolve())

    with Beebium.launch(
        server_filepath=str(BEEBIUM_SERVER),
        mos_filepath=str(MOS_ROM),
        basic_filepath=str(BASIC_ROM),
        extra_args=[
            "--fdc", "acorn-1770",
            "--floppy", f"0:{disc_path}",
            "--sideways", f"14:rom:{dfs_rom_path}",
        ],
    ) as bbc:
        bbc.debugger.stop()
        bbc.debugger.reset()
        bbc.debugger.run()
        time.sleep(2.0)
        bbc.debugger.stop()

        rows = read_mode7_screen(bbc)
        boot_screen = "\n".join(rows)
        print(f"\n--- Boot Screen ---\n{boot_screen}\n-------------------")

        bbc.keyboard.type("*HELLO\n")
        bbc.debugger.run()
        time.sleep(1.5)
        bbc.debugger.stop()

        rows = read_mode7_screen(bbc)
        screen_text = "\n".join(rows)
        print(f"\n--- After *HELLO ---\n{screen_text}\n--------------------")

        assert screen_contains(bbc, "HELLO"), (
            f"Expected 'HELLO' on screen after running *HELLO\n"
            f"Screen:\n{screen_text}"
        )