# Memory Lab: Arena Allocator and Inspector

This is the companion implementation for the [20-lesson memory course](../../../docs/tutorials/courses/memory-lab/README.md). It is intentionally more than a one-file pointer demo: it owns a backing allocation, reserves aligned subregions, records allocation metadata, tracks payload and padding separately, checks invariants, writes deterministic patterns, and verifies failure atomicity.

## Project layout

- `src/arena.vnt` — backing allocation, aligned reservations, metadata, reset/destroy, counters, invariant checks.
- `src/inspect.vnt` — deterministic byte patterns, round-trip verification, copy-and-compare.
- `src/report.vnt` — human-readable summary and allocation table.
- `src/main.vnt` — integration demonstration with multiple phases.
- `tests/smoke.vnt` — byte round-trip and accounting check.
- `tests/bounds.vnt` — invalid request and failure-atomicity regression checks.

## Build and run

From the repository root, build the compiler using the root README. Then compile the entry point:

```powershell
.\\vnt.exe --compile examples\\courses\\memory-lab\\src\\main.vnt -o memory-lab.exe
.\\memory-lab.exe
```

The import loader expands source modules relative to the importing file. If the compiler reports a syntax or import error, compare the exact syntax with current tests and standard-library source; VNT is experimental.

Compile the tests independently using the same command pattern:

```powershell
.\\vnt.exe --compile examples\\courses\\memory-lab\\tests\\smoke.vnt -o memory-lab-smoke.exe
.\\memory-lab-smoke.exe
.\\vnt.exe --compile examples\\courses\\memory-lab\\tests\\bounds.vnt -o memory-lab-bounds.exe
.\\memory-lab-bounds.exe
```

The tests print explicit PASS/FAIL markers. Treat any FAIL marker as a failed test even if the process exits normally.

## Invariants and design choices

- The arena owns one allocation; callers only receive pointers to reserved ranges.
- Cursor, payload bytes, and padding bytes are tracked separately.
- Reservation validation happens before metadata or cursor changes.
- Bounds checks use `size <= capacity - start` after validating `start <= capacity`, avoiding overflow in `start + size` during the check.
- Supported alignments are deliberately limited to 1, 2, 4, 8, and 16 bytes.
- Reset starts a new logical lifetime. All previously returned pointers must be treated as invalid after reset.
- Destroy releases the backing allocation and clears accounting.
- The allocation table is a bounded teaching log of successful reservations, not a full heap profiler.

## Limitations

This is an educational hosted allocator, not a production heap or kernel allocator. It does not support individual free, thread safety, reallocation of arena regions, guard pages, leak-safe ownership tracking, or arbitrary alignment beyond the documented set. Raw pointers are not bounds checked by the language. Do not pass arbitrary addresses to the memory helpers or use this project for hardware access.

The course asks you to extend the implementation with exact-fit tests, reset/reuse cases, and additional invariants. Do not claim correctness from a single successful run.
