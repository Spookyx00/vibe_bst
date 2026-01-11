# Safety-Critical Binary Search (C11)

This repository contains a reference implementation of binary search primitives (`lower_bound`, `upper_bound`, `binary_search`) designed for safety-critical systems (e.g., flight software).

**File:** `safesearch.c`

## Key Features

*   **Strict C11 Compliance**: Compiles cleanly with `-std=c11 -Wall -Wextra -Wpedantic`.
*   **Defined Behavior Only**: Guaranteed absence of strict aliasing violations, signed integer overflows, or invalid pointer arithmetic.
*   **Safety First**:
    *   **No Recursion**: Prevents stack overflow risks.
    *   **No Dynamic Memory**: No `malloc`/`free`.
    *   **Overflow-Safe Midpoint**: Uses `mid = lo + (hi - lo) / 2` to avoid wrapping for large indices.
    *   **Robust Error Handling**: Explicit `status_t` enums for results; no sentinel values like `-1`.
*   **Deterministic**: Handles duplicates consistently (always finds the *first* occurrence or insertion point).

## API Overview

The implementation provides three main functions operating on `int32_t` arrays:

1.  `search_result_t lower_bound_i32(const int32_t* a, size_t n, int32_t key)`
    *   Finds the first position `i` where `a[i] >= key`.
2.  `search_result_t upper_bound_i32(const int32_t* a, size_t n, int32_t key)`
    *   Finds the first position `i` where `a[i] > key`.
3.  `search_result_t binary_search_i32(const int32_t* a, size_t n, int32_t key)`
    *   Returns `OK_FOUND` and index if present, or `OK_NOT_FOUND` and insertion index if missing.

## Verification

The source file includes a built-in test harness that runs automatically when compiled and executed.

### Test Coverage
*   **Fixed Vectors**: Edge cases including empty arrays, single elements, all-equals, and `INT32_MIN`/`MAX`.
*   **Property-Based Testing**: Runs ~200,000 randomized queries against a linear scan oracle to verify:
    *   **Correctness**: Results match reference linear scan exactly.
    *   **Invariants**: Post-conditions hold for every result.
    *   **No Crashes**: Validates robustness against random inputs.

### Usage

To compile and run the verification suite:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -o safesearch safesearch.c
./safesearch
```

Expected output:
```text
Running Fixed Vector Tests...
Running Property-Based Tests...

SUMMARY: PASS
Tests Run: 200024

SUMMARY: PASS
Tests Run: 200024
Tests Passed: 200024
```

### Advanced Fuzz Testing

For safety-critical assurance, we provide a fuzz testing harness using LLVM's `libFuzzer` combined with AddressSanitizer (ASAN) and UndefinedBehaviorSanitizer (UBSAN).

**Prerequisites**: `clang`

**Fuzz Target**: `fuzz_safesearch.c`

To run the fuzzer:

```bash
# Compile with sanitizers and fuzzer driver
clang -g -O1 -fsanitize=fuzzer,address,undefined -DSAFE_SEARCH_LIB safesearch.c fuzz_safesearch.c -o fuzz_safesearch

# Run (e.g., for 10 seconds)
./fuzz_safesearch -max_total_time=10
```

The fuzzer generates millions of random input vectors, sorts them (to satisfy preconditions), and validates the invariants of `lower_bound`, `upper_bound`, and `binary_search`. It continuously checks for:
*   Crashes / Segfaults
*   Memory Safety violations (ASAN)
*   Undefined Behavior (UBSAN)
*   Logical consistency against local property checks

