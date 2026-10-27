# VNT benchmark suite

The suite times separate runtime workloads rather than treating one loop as a
complete language ranking. It compiles workloads before timing, validates
expected output where stable, performs warmups, then reports median/min/max.

## Coverage

| Workload | Main features exercised |
| --- | --- |
| integer-loop | Integer arithmetic, reassignment, while loops |
| branches-logic | Comparisons, modulo, nested conditionals, && and || |
| function-calls | Function calls, four parameters, hot-loop arithmetic |
| recursion | Recursive calls and return values |
| strings | String concatenation and len |
| arrays | Array literals, indexing, indexed mutation, len |
| floats-math | Floating-point arithmetic and math builtins |
| references | Address-of, dereference, reference assignment |
| structs | Struct creation and member reads/writes |
| many-arguments | Eight-parameter function call |
| ffi | Windows kernel32.dll FFI call |
| multi-file | Two imported VNT modules and cross-file function calls |

References, structs, argument passing, and FFI are separate on purpose. If one
feature crashes or fails, the runner now identifies that exact workload rather
than hiding the cause inside a combined program. The FFI test targets Windows.

## Run in PowerShell

From the repository root:

    .\build.ps1
    .\benchmarks\run.ps1

Optional:

    .\benchmarks\run.ps1 -Runs 11 -Warmups 2

Executables and results.csv are written to benchmarks\out. That folder is
generated output and should not be committed.

Compilation and correctness checks are outside timed measurements. Timing
includes launching each native executable, so short workloads may be dominated
by process startup. Compare medians on the same machine and build.

For the existing C/Python integer-loop comparison, see ../README.md.
