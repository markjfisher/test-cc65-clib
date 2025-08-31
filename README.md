# BBC CLIB ROM Test Framework

This testing framework validates the **bbc-clib** ROM-based target for cc65, which provides tiny BBC Micro applications that call functions from a sideways ROM instead of static linking.

## Architecture

```
test-cc65-clib/
├── build-rom/          # ROM building infrastructure
│   └── Makefile        # Builds clib.rom from ../cc65-clib/
├── roms/               # Generated ROM files  
│   ├── clib.rom        # 8.6KB ROM image
│   ├── clib.lib        # 492KB stub library
│   └── clib.map        # Symbol addresses
├── tests/              # Test programs
│   ├── test-strings/   # String functions (strlen, strcpy)
│   ├── test-maths/     # Math functions (abs, labs, itoa)
│   └── ...
└── build.sh           # Main build script
```

## Quick Start

### Build Everything
```bash
./build.sh              # Builds ROM + all tests
```

### Build Individual Components  
```bash
./build.sh rom          # Build ROM only
./build.sh tests        # Build all tests only
./build.sh clean        # Clean everything
```

### Build Individual Test
```bash
cd tests/test-basic
make all               # Build single test
```

## Test Programs

### test-basic
Tests core functions available in ROM:
- `abs(-42)` → `42`
- `atoi("123")` → `123`
- Basic arithmetic

### test-strings  
Tests string manipulation in ROM:
- `strlen("Hello")` → `5`
- `strcpy()`, `strcat()`, `strcmp()`
- `strchr()` character search

### test-math
Tests mathematical functions in ROM:
- `abs()`, `labs()` for absolute values
- `atoi()`, `atol()` string to number conversion
- `itoa()`, `ltoa()` number to string conversion

### test-rom-detection
Tests ROM detection mechanism:
- Verifies `clib_rom_available` flag
- Tests `detect_clib_rom()` function
- Validates ROM startup process

## Running Tests

1. **Build ROM and tests**: `./build.sh`
2. **Load ROM**: Install `roms/clib.rom` in BBC emulator sideways slot 1  
3. **Run test**: Load any `test` executable and run it

## Size Comparison

**Traditional `bbc` target:**
- Application + ~8-15KB static library = Large executable

**ROM `bbc-clib` target:**  
- Application + ~200-500 bytes stubs = **90% smaller!**
- Requires `clib.rom` to be present

## Technical Details

- **ROM Functions**: 108+ C library functions (strlen, strcpy, memcpy, abs, atoi, etc.)
- **ROM Size**: 8.6KB (fits in single sideways ROM slot)
- **Stub Size**: ~200-500 bytes per application (vs 8-15KB static linking)
- **Detection**: Automatic ROM detection at startup with error handling
- **Compatibility**: Works alongside traditional `bbc` target

This framework provides comprehensive testing for the revolutionary ROM-based BBC Micro development approach!