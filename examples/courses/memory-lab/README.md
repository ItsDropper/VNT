# Memory Lab: Arena Allocator and Inspector

This is the companion implementation for the [20-lesson memory course](../../../docs/tutorials/courses/memory-lab/README.md). It is an actual multi-file arena implementation rather than a one-file pointer demo: it owns a backing allocation, reserves aligned subregions, records allocation metadata, tracks payload and padding separately, checks invariants, writes deterministic patterns, and verifies failure atomicity.

## Project layout

- `src/arena.vnt` — backing allocation, aligned reservations, metadata, reset/destroy, counters, invariant checks.
- `src/inspect.vnt` — deterministic byte patterns, round-trip verification, copy-and-compare.
- `src/report.vnt` — human-readable summary and allocation table.
- `src/main.vnt` — integration demonstration with multiple phases.
- `tests/smoke.vnt` — byte round-trip and accounting check.
- `tests/bounds.vnt` — invalid request and failure-atomicity regression checks.

## Build and run with VBuild

VBuild gives this multi-file project repeatable build, run, and test tasks. It does not need Python. Build VBuild once from the repository root:

```powershell
gcc -std=c11 -O2 tools\vbuild.c -o vbuild.exe
```

On Linux or WSL, use `gcc -std=c11 -O2 tools/vbuild.c -o vbuild` instead.

Because this example already has source files, do **not** run `vbuild init` inside it: initialization refuses to overwrite existing files. Create these two project files in `examples/courses/memory-lab/`.

`vbuild.properties`:

```properties
project.name = memory-lab
project.version = 0.1.0
source = src/main.vnt
output = memory-lab
compiler = vnt
toolchain = ../../..
```

The `toolchain` path points from this example directory back to the VNT repository root. Keep it as `../../..` when building from this checked-in location; for a copied project, set it to your actual VNT checkout path or configure `VNT_HOME`.

`build.vbuild`:

```text
target exe
task build = build
task run = run depends build
task check = check
task test = test
task clean = clean
task verify = none depends check, build, test
```

Run these commands from `examples/courses/memory-lab/`:

```powershell
..\..\..\vbuild.exe tasks
..\..\..\vbuild.exe build
..\..\..\vbuild.exe run
..\..\..\vbuild.exe test
..\..\..\vbuild.exe verify
```

On Linux or WSL, replace `..\..\..\vbuild.exe` with `../../../vbuild`. `build` writes the executable under `build/`; `run` builds first and executes it; `test` discovers and runs the top-level `tests/*.vnt` programs. The test programs must return a non-zero exit status when an assertion fails for VBuild to detect the failure. If a test only prints `FAIL` and exits successfully, VBuild cannot infer that it failed; inspect its output and improve the test's exit behavior.

VBuild's incremental build fingerprint includes the root source, imported VNT modules, explicitly declared dependency modules, task file, project properties, and relevant compiler/runtime inputs. After changing system libraries or environment variables, use `..\..\..\vbuild.exe build --force` to avoid relying on inputs that VBuild does not fingerprint.

## Direct compiler fallback

If you are learning the compiler command itself, or diagnosing a VBuild issue, you can still compile directly from the repository root:

```powershell
.\vnt.exe --compile examples\courses\memory-lab\src\main.vnt -o memory-lab.exe
.\memory-lab.exe
```

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
