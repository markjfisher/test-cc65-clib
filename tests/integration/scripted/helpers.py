"""
Shared test helpers for cc65-bbc integration tests.

Provides:
- cc65_launch() context manager — starts beebium with correct args
- build_binary() — compile .c -> binary -> SSD
- get_screen_text() / assert_screen_contains() — MODE 7 screen helpers
"""

from __future__ import annotations

import subprocess
import time
from pathlib import Path

from beebium import Beebium
from beebium.screen import read_mode7_screen, screen_contains


# --- paths ----------------------------------------------------------------
PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent.parent
CC65_CLI = PROJECT_ROOT.parent / "cc65" / "bin" / "cl65"
DISCS_DIR = PROJECT_ROOT / "tests" / "integration" / "discs"
DFSTOOL = Path.home() / ".cargo" / "bin" / "dfstool"
BEEBIUM_SERVER = Path("/home/markf/dev/bbc/beebium/build-release/src/server/beebium-model-b")
DFS_ROM = Path("/home/markf/dev/bbc/beebium/build-release/roms/acorn-dfs_2_26.rom")
MOS_ROM = Path.home() / ".local/share/beebium/roms/acorn-mos_1_20.rom"
BASIC_ROM = Path.home() / ".local/share/beebium/roms/bbc-basic_2.rom"


def check_prereqs():
    """Return list of missing dependencies, or empty list if all OK."""
    required = {
        "MOS ROM": MOS_ROM,
        "BASIC ROM": BASIC_ROM,
        "DFS ROM": DFS_ROM,
        "beebium-server": BEEBIUM_SERVER,
        "dfstool": DFSTOOL,
        "cc65": CC65_CLI,
    }
    return [name for name, p in required.items() if not p.exists()]


def cc65_launch(c_filename, extra_args=None):
    """
    Context manager that:
    1. Compiles the .c file with cc65 -t bbc
    2. Packages into a DFS SSD
    3. Starts beebium with DFS loaded
    4. Yields the Beebium instance after booting
    """
    c_path = Path(c_filename)
    if not c_path.is_absolute():
        c_path = DISCS_DIR / c_path.name
    stem = c_path.stem.upper()[:10]
    bin_path = DISCS_DIR / stem
    ssd_path = DISCS_DIR / f"{stem}.ssd"
    json_path = DISCS_DIR / f"{stem}.json"

    # Compile
    subprocess.run(
        [str(CC65_CLI), "-t", "bbc", "-o", str(bin_path), str(c_path)],
        check=True, capture_output=True,
    )

    # Determine load address from the binary labels
    result = subprocess.run(
        [str(CC65_CLI), "-t", "bbc", "-Ln", str(bin_path.with_suffix(".lbl")),
         "-o", str(bin_path), str(c_path)],
        check=True, capture_output=True,
    )

    # Find __Cstart in the label file to get load address
    lbl_path = bin_path.with_suffix(".lbl")
    load_addr = 0x0E00  # default bbc load address
    if lbl_path.exists():
        for line in lbl_path.read_text().splitlines():
            parts = line.split()
            if len(parts) >= 3 and parts[2] == ".__Cstart":
                load_addr = int(parts[1], 16)

    # Write manifest
    json_path.write_text(
        "{{\n"
        '  "version": 1,\n'
        f'  "discTitle": "{stem}",\n'
        '  "discSize": 800,\n'
        '  "bootOption": "none",\n'
        '  "cycleNumber": 0,\n'
        '  "files": [\n'
        "    {{\n"
        f'      "fileName": "{stem}",\n'
        '      "directory": "$",\n'
        '      "locked": false,\n'
        f'      "loadAddress": "&{load_addr:06X}",\n'
        f'      "executionAddress": "&{load_addr:06X}",\n'
        f'      "contentPath": "{stem}",\n'
        '      "type": "other"\n'
        "    }}\n"
        "  ]\n"
        "}}"
    )

    subprocess.run(
        [str(DFSTOOL), "make", "--output", str(ssd_path), "--overwrite",
         str(json_path)],
        check=True, capture_output=True,
    )

    ssd_path_str = str(ssd_path.resolve())
    dfs_rom_path = str(DFS_ROM.resolve())

    combined_extra = [
        "--fdc", "acorn-1770",
        "--floppy", f"0:{ssd_path_str}",
        "--sideways", f"14:rom:{dfs_rom_path}",
    ]
    if extra_args:
        combined_extra.extend(extra_args)

    return Beebium.launch(
        server_filepath=str(BEEBIUM_SERVER),
        mos_filepath=str(MOS_ROM),
        basic_filepath=str(BASIC_ROM),
        extra_args=combined_extra,
    )


def boot_and_run(bbc, cmd, settle=1.5):
    """Reset, boot to BASIC, type a *command, and wait."""
    bbc.debugger.stop()
    bbc.debugger.reset()
    bbc.debugger.run()
    time.sleep(settle)
    bbc.debugger.stop()
    bbc.keyboard.type(f"{cmd}\n")
    bbc.debugger.run()
    time.sleep(1.0)
    bbc.debugger.stop()


def get_screen_text(bbc):
    """Return the full MODE 7 screen as a single string."""
    rows = read_mode7_screen(bbc)
    return "\n".join(rows)


def assert_screen_contains(bbc, text):
    """Assert that text appears anywhere on screen."""
    assert screen_contains(bbc, text), (
        f"Expected {text!r} on screen\n"
        f"Screen:\n{get_screen_text(bbc)}"
    )