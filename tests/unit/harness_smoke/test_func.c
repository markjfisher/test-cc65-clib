/*
 * Minimal test to verify the soft65c02 harness works.
 * Tests that a simple C function (returning a constant) executes correctly.
 * This is a "smoke test" for the test infrastructure itself.
 */

int test_func(void) {
    return 42;
}