# test-cc65-clib agent instructions

## Project Context

This project provides a test harness for the **cc65 fork** bbc target
(`/home/markf/dev/bbc/cc65`). It tests cc65's BBC Micro C library functions
via two emulator-based test tiers, modelled after the `fn-rom` project
(`/home/markf/dev/nio/repos/fn-rom`).

### Related projects

| Project | Path | Purpose |
|---------|------|---------|
| cc65 fork | `/home/markf/dev/bbc/cc65` | cc65 with bbc and bbc-clib targets |
| cc65-clib | `/home/markf/dev/bbc/cc65-clib` | ROM-split C library for bbc-clib target |
| fn-rom | `/home/markf/dev/nio/repos/fn-rom` | Reference project for test patterns |

## Test Architecture

### Tier 1: soft65c02 unit tests

Fast, isolated tests that run individual cc65 library functions in a pure
6502 emulator. No BBC hardware emulation needed.

- **Tool**: `soft65c02_unit` (Rust binary, expected at `/home/markf/.cargo/bin/soft65c02_unit`)
- **Harness**: `tests/harness/` — minimal BBC MOS stub that calls a test function
  and halts (`$db` = STP opcode)
- **Test definition**: YAML file (`*.yaml`) + test script (`*.txt`)
- **Test scripts** use the soft65c02 DSL: `memory load`, `registers flush`,
  `run <label> until (...)` , `assert ...`

### Tier 2: Beebium integration tests

Full BBC Micro emulation via beebium with gRPC control. Tests end-to-end
behavior (OS calls, file I/O, screen output, serial I/O).

- **Tool**: `beebium` CLI + pytest
- **Location**: `tests/integration/`

## Running Tests

```bash
./run_tests.sh                  # full matrix
./run_tests.sh --no-beebium     # builds + unit tests only
./run_unit_tests.sh             # soft65c02 unit tests only
./run_integration_tests.sh      # beebium integration tests only
```

## Adding a Unit Test

1. Create a directory under `tests/unit/<test-name>/`
2. Write a `.s` file implementing the C test function (`_test_func`) that:
   - Receives test data via memory writes from the test script
   - Calls the library function under test
   - Stores results in known memory locations
   - Returns to the harness crt0 which hits `brk`
3. Write a `.yaml` file referencing the harness and test sources:
   ```yaml
   name: test description
   configs:
     - ${HARNESS_DIR}/harness.yaml
   src_files:
     - test_<name>.s
   test_script: test_<name>.txt
   ```
4. Write a `.txt` test script with DSL commands:
   ```
   memory load atari "${BINARY_PATH}"
   symbols load "${SYMBOLS_PATH}"
   registers flush
   memory write <addr> <value>     ; set up test data
   run $init_harness until (...)
   assert <condition>  $$description$$
   ```

## Build Process

The cc65 bbc library must be built before running tests. This is done via:

```bash
make -C build-rom rebuild-cc65-lib
```

The harness binary is auto-built by `soft65c02_unit` using the config in
`harness.yaml`. No manual build step needed.

## Adding a Beebium Integration Test

1. Create a Python file under `tests/integration/scripted/`
2. Use the `beebium` pytest fixture from `conftest.py`
3. Write tests that:
   - Load a C test binary via `*RUN` or `*TEST`
   - Scrape screen output via the beebium gRPC API
   - Assert expected output text

## Project Structure

```
test-cc65-clib/
├── run_tests.sh                  # Full test matrix entry point
├── run_unit_tests.sh             # soft65c02 unit tests runner
├── run_integration_tests.sh      # Beebium integration tests runner
├── test_env.sh                   # Environment variables for soft65c02
├── tests/
│   ├── harness/                  # Soft65c02 unit test harness
│   │   ├── harness.yaml          # soft65c02_unit config
│   │   ├── cfg/harness.ld65.cfg  # Linker config
│   │   └── src/
│   │       ├── crt0.s            # Entry: calls _test_func, hits brk
│   │       ├── osbyte_stub.s     # OSBYTE dispatch table
│   │       ├── print_mock.s      # OSWRCH capture buffer
│   │       ├── mos_vectors.s     # MOS vector table
│   │       └── stubs.s           # Unimplemented vectors -> brk
│   ├── unit/                     # soft65c02 unit tests
│   │   └── <test-name>/
│   │       ├── test_<name>.yaml  # Test config
│   │       ├── test_<name>.txt   # Test DSL script
│   │       └── test_<name>.s     # Assembly test source
│   └── integration/              # Beebium integration tests
│       ├── pyproject.toml
│       ├── conftest.py
│       └── scripted/
│           └── test_*.py
├── tests/                        # Legacy b2-based tests
│   ├── test-break-handler/
│   ├── test-c-comprehensive/
│   ├── test-files/
│   ├── test-maths/
│   ├── test-serial/
│   └── test-strings/
└── build.sh                      # Legacy build script
```

## Legacy Tests

The tests in `tests/test-*` are the original b2-based visual tests. They are
not automated. New tests should be added to `tests/unit/` or
`tests/integration/` using the soft65c02 or beebium frameworks.

## Test Plan (from initial review)

Full plan is in AGENTS.md review above. In priority order:

1. Port existing test coverage to soft65c02 unit tests
2. Add edge case coverage for each cc65 bbc library function
3. Add beebium integration tests for end-to-end application scenarios
4. Add cycle-count assertions for performance-critical functions
5. Cross-profile testing (bbc vs bbc-clib targets)

Each cc65 library function in `/home/markf/dev/bbc/cc65/libsrc/bbc/` should
have a corresponding unit test in `tests/unit/`.