# VBuild

VBuild is VNT's native C build driver. It does real work: it invokes the VNT
compiler, selects its native or freestanding backend, writes artifacts under
`build/`, skips builds whose tracked inputs have not changed, runs built
programs, and discovers and executes project test programs. It has no Python
runtime dependency.

## Build VBuild

From the VNT repository root:

```sh
gcc -std=c11 -O2 tools/vbuild.c -o vbuild
```

On Windows with GCC:

```powershell
gcc -std=c11 -O2 tools/vbuild.c -o vbuild.exe
```

## Create a project

```powershell
.\\vbuild init
.\\vbuild tasks
.\\vbuild build
.\\vbuild run
.\\vbuild check
.\\vbuild test
.\\vbuild clean
```

On Linux use `./vbuild`. Initialization creates `build.vbuild`,
`vbuild.properties`, and `src/main.vnt`. It refuses to overwrite existing
project files. For projects outside the VNT checkout, set `toolchain` in
`vbuild.properties` to the VNT repository root, or omit it and set
`VNT_HOME`.

## Select the artifact in build.vbuild

The top-level `target` directive selects what the build task produces:

```text
# VBuild task file (version 1)
target exe
task build = build
task run = run depends build
task check = check
task test = test
task clean = clean
task verify = none depends check, build
```

Supported targets:

| Target | Output | Compiler operation |
|---|---|---|
| `exe` | Native executable (`.exe` on Windows) | `vnt --compile` with VNT's hosted runtime |
| `elf` | Freestanding x86-64 ELF (`.elf`) | `vnt --build-freestanding` |
| `asm` | Native assembly (`.s`) | `vnt --emit-asm` |
| `freestanding-asm` | Freestanding assembly (`.s`) | `vnt --emit-freestanding-asm` |

For example, change `target exe` to `target elf` to build a freestanding ELF.
The ELF target requires Linux/WSL with GCC, GNU `ld`, and the VNT repository's
`freestanding/x86_64.ld` linker script. The existing VNT freestanding backend
supports a restricted language/runtime subset; a successful build does not
mean hosted APIs or every language feature is available in a kernel.

`run` is intentionally limited to `target exe`; it refuses to pretend an
assembly file or freestanding kernel is a hosted application. The `output`
property is the artifact basename, and VBuild appends the target's conventional
extension when needed.

## Task syntax and dependencies

```text
task NAME = ACTION [depends TASK[, TASK...]]
```

Supported actions are `build`, `run`, `check`, `clean`, `test`, and
`none`. The `none` action groups dependencies without running another
action. A dependency runs at most once per invocation. Undefined dependencies,
duplicate task names, invalid actions, and dependency cycles are rejected.
Custom shell commands are intentionally unsupported.

## vbuild.properties

```properties
project.name = hello
project.version = 0.1.0
source = src/main.vnt
output = hello
compiler = vnt
toolchain = C:/path/to/VNT
```

Supported properties are `project.name`, `project.version`, `source`,
`output`, `compiler`, and `toolchain`. `toolchain` is the VNT repository
root. VBuild launches the compiler from that directory so the compiler can
resolve its native runtime and linker-script paths. `source` is the root VNT
file; VNT's module loader handles imports.

## Incremental builds

VBuild checks the output timestamp against the configured root source, project
properties, task file, and compiler executable. If the output is current, it
prints `Up to date` instead of invoking the compiler. Use
`vbuild build --force` to force recompilation.

This first incremental implementation does not yet fingerprint imported VNT
modules or every native runtime/linker input. If you change an imported module
or a runtime file and the output appears current, force a rebuild. Full
dependency-graph tracking is a follow-up.

## Tests

Put passing, standalone VNT programs in the project's `tests/` directory,
for example `tests/smoke.vnt`. `vbuild test` discovers the top-level
`*.vnt` files there, compiles each as a hosted executable, runs it, and fails
if compilation or execution returns a non-zero status. Tests should return
success when their assertions pass. Test discovery is non-recursive; nested
directories and expected-output assertions are not yet supported.

## Current boundaries

- VBuild is a native C executable; a C compiler is needed to build VBuild from
  source, but running a built VBuild binary does not need Python.
- One root source file is configured per project; VNT's module loader handles
  imports.
- Test programs are standalone passing executables, not a framework with
  assertions, fixtures, or expected-output files.
- Incremental dependency tracking, profiles, cross-compilation, arbitrary
  custom tasks, and reproducible-build guarantees are not implemented.
