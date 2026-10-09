# VNT benchmark suite

The suite times separate runtime workloads rather than treating one loop as a
complete language ranking. It compiles workloads before timing, checks a stable
output marker, performs warmups, then reports median/min/max across process launches.

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
| structs-refs-ffi | Structs, member access, references, eight arguments, Windows FFI |
| multi-file | Two imported VNT modules and cross-file function calls |

This is broad coverage of currently implemented features, not proof that every
edge case is tested. The FFI workload calls Windows kernel32.dll, so the suite
targets Windows. Run on an idle machine for more stable timings.

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
