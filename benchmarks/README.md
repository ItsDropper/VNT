# VNT benchmarks

VNT has two benchmark paths:

- **Feature suite:** broad runtime coverage for arithmetic, branching, calls, recursion, strings, arrays, floats, references, structs, FFI, and multi-file projects.
- **Native comparison:** the same integer-loop workload compiled by VNT, GCC, and Rust, with correctness checks, warmups, repeated timings, medians, and CSV output.

## Run the VNT feature suite

From the repository root in PowerShell:

    .\build.ps1
    .\benchmarks\run.ps1

For more samples and warmups:

    .\benchmarks\run.ps1 -Runs 11 -Warmups 2

The feature runner compiles each VNT workload before timing and checks its stable output marker. It writes benchmarks\out\results.csv. The FFI test currently targets Windows.

## Compare VNT, C, and Rust

Build VNT first, then run:

    .\build.ps1
    .\benchmarks\compare.ps1

Optional:

    .\benchmarks\compare.ps1 -Runs 9 -Warmups 2

The comparison requires gcc and rustc on PATH. It compiles:

- VNT with the VNT native compiler and GCC -O3 -flto runtime link.
- C with GCC -O3 -flto; a compiler barrier prevents dead-loop elimination without forcing every loop variable to memory.
- Rust with rustc -C opt-level=3 -C lto=thin -C codegen-units=1; std::hint::black_box keeps the loop observable.

All three execute exactly 20 rounds of 1,000,000 integer iterations and must print 499999500000. The VNT workload runs inside a function so its variables are eligible for local scalar optimization rather than global storage. Its hot loop calls a tiny pure helper (`add_value`); HIR inlining must remove that abstraction before native code generation so the generated loop remains comparable to the direct C/Rust loops. The compiler also proves the positive-step induction variable and zero-based sum accumulator stay within signed 64-bit range, removing their overflow branches; the comparison script checks both assembly properties before timing. The hosted x86-64 backend conservatively keeps eligible non-escaping integer locals unboxed, uses direct arithmetic/comparison instructions, and allocates up to four locals to preserved registers; other values remain boxed or stack-backed. The runner refuses to report timings if any output differs. Compilation is outside the timed region; process startup is included. It performs warmups, reports every sample plus median/min/max, and writes benchmarks\out\comparison.csv.

This is one integer-heavy microbenchmark, not a language-wide ranking. It does not establish parity on allocation, strings, arrays, FFI, compile time, or code size. Run on the same machine with the same toolchain versions and a quiet system; compare multiple runs. The existing Python implementation is retained as a separate reference, not included in the native comparison.

## Benchmark suite coverage

See suite/README.md for the feature matrix and caveats. The full feature runner measures native executable runtime, while the comparison runner isolates one equivalent loop workload across three compilers.
