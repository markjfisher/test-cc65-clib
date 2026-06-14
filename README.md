# RETIRED

This project is retired, and has been fully merged into https://github.com/markjfisher/cc65-clib

It is kept only for the history which was not merged.

---

## Test-cc65-clib: Test Suite for cc65 BBC Micro Target

This project tests the cc65 fork's BBC Micro target (`-t bbc`)
at `/home/markf/dev/bbc/cc65`. It uses two emulator-based test tiers.

## Project Structure

```
test-cc65-clib/
├── run_tests.sh                  # Full test matrix entry point
├── run_unit_tests.sh             # soft65c02 unit tests
├── run_integration_tests.sh      # Beebium integration tests
├── test_env.sh                   # Environment for unit tests
├── AGENTS.md                     # Agent/developer instructions
│
├── tests/harness/                # soft65c02 unit test harness
│   ├── harness.yaml              # soft65c02_unit config
│   ├── cfg/harness.ld65.cfg      # Linker config
│   └── src/
│       ├── crt0.s                # Entry: calls _test_func, hits brk
│       ├── osbyte_stub.s         # OSBYTE dispatch + mocks
│       ├── print_mock.s          # OSWRCH capture buffer
│       ├── mos_vectors.s         # MOS vector table
│       └── stubs.s               # Unimplemented vectors -> brk
│
├── tests/unit/                   # soft65c02 unit tests
│   ├── strlen/                   # One directory per function
│   │   ├── test_strlen.s
│   │   ├── test_strlen.yaml
│   │   └── test_strlen.txt
│   ├── abs/
│   ├── break_handler/
│   └── ... ~50 test dirs
│
├── tests/integration/            # Beebium integration tests
│   ├── pyproject.toml            # Python deps (beebium, pytest)
│   ├── conftest.py               # Pytest config
│   ├── discs/                    # C test program sources (NOT build artifacts)
│   │   ├── hello.c
│   │   ├── test_console.c
│   │   ├── test_fileio.c
│   │   ├── test_clock.c
│   │   ├── test_break.c
│   │   ├── test_screen.c
│   │   ├── test_kbhit.c
│   │   └── brk_helper.s
│   ├── scripted/                 # Pytest test files
│   │   └── test_basic.py
│   └── scripts/                  # Build scripts
│       └── build_test_discs.sh
│
└── build/                        # Build artifacts (all generated)
    └── integration-testing/discs/
        ├── TCONSL.ssd
        ├── TBREAK.ssd
        └── ... (generated, not committed)
```

## Test Architecture

### Tier 1: soft65c02 unit tests (fast, isolated)

Tests individual cc65 library functions in a pure 6502 emulator. No BBC hardware.

- **Tool**: `soft65c02_unit` (Rust, at ~/.cargo/bin/)
- **Pattern**: Assembly `.s` file calls library functions, stores results.
  YAML config + DSL `.txt` script drives the emulator.
- **Run**: `./run_unit_tests.sh` — discovers all `tests/unit/*/test_*.yaml`
- **Add**: Create a new directory under `tests/unit/`. See `AGENTS.md` for details.

### Tier 2: Beebium integration tests (full BBC emulation)

End-to-end tests using the beebium BBC Model B emulator with gRPC control.

- **Tool**: `beebium-model-b` + Python `beebium` client
- **Pattern**: C programs compiled with `cc65 -t bbc`, loaded from DFS disc.
  Pytest scripts type commands, read screen output, assert results.
- **Run**: `./run_integration_tests.sh`
- **Add**: Write a `.c` test program in `tests/integration/discs/`, a corresponding
  pytest in `tests/integration/scripted/` (use the `clib_harness` module), and add
  it to `build_test_discs.sh`.
- **Important**: test programs are linked at `$1900` (cc65 `--start-addr 0x1900`),
  not the cc65 bbc default of `$0E00`. `$0E00` collides with Acorn DFS private
  workspace, so a program *RUN from a DFS disc there silently corrupts DFS and
  produces no screen output. `build_test_discs.sh` handles this; the disc
  catalogue load/exec addresses must match (`&001900`).
- The harness runs the emulator at unlimited speed (`set_speed_multiplier(0.0)`)
  so disc I/O completes quickly, and reads the scroll-corrected MODE 7 screen via
  `beebium.screen` (which already accounts for 6845 hardware scrolling).

## Running Tests

### Full matrix (recommended)

```bash
./run_tests.sh              # all tiers
./run_tests.sh --no-beebium # skip integration if beebium not set up
```

### Unit tests only

```bash
./run_unit_tests.sh                  # all
./run_unit_tests.sh strlen           # specific test
```

### Integration tests only

```bash
./run_integration_tests.sh                    # all
BEEBIUM_SERVER=... ./run_integration_tests.sh # with custom server
```

The integration test runner auto-discovers dependencies from the sibling
`../beebium` checkout. Set these env vars to override:

| Variable | Purpose | Default |
|----------|---------|---------|
| `BEEBIUM_SERVER` | Path to `beebium-model-b` | `../beebium/build-release/...` |
| `BEEBIUM_ROM_DIR` | ROM files directory | `../beebium/build-release/roms` |
| `CC65_DIR` | cc65 checkout directory | `../cc65` |

### Requirements

- **cc65 bbc library**: Rebuilt by `run_tests.sh` step 1
- **soft65c02_unit**: Install Rust version from project repo
- **beebium**: Build from sibling checkout `../beebium`
- **dfstool**: Install from https://github.com/rcook/dfstool
- **Python**: venv at `.venv/` with `beebium` and `pytest` installed

## Converting Legacy Tests

The original `tests/test-*` directories were b2-based visual tests (no automation).
They should NOT be used. New tests should be added as:
- soft65c02 unit tests in `tests/unit/` for individual library functions
- Beebium integration tests in `tests/integration/` for full programs