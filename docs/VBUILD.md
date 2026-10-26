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

## Local repositories and dependencies

VBuild supports source-module dependencies from local repositories. This uses
the same block-oriented style as Gradle, but it is not a Maven/Maven Central
client and does not download packages.

```text
repositories {
    local("libs")
}

dependencies {
    implementation("math_helpers.vnt")
}
```

Here, `libs/` is a project-relative directory, and
`libs/math_helpers.vnt` is the declared module. Import it in VNT source using
the module's file name:

```vnt
import "math_helpers.vnt" as math_helpers
print(math_helpers.square(7))
```

The compiler first resolves imports relative to the importing file, then
searches each configured local repository. VBuild validates that every declared
`implementation` module exists before compiling and adds repository paths to
`VNT_MODULE_PATH` for the compiler process. Repository and dependency paths
must be relative and cannot traverse outside the project/repository.

Only `local("directory")` repositories and `implementation("relative-file.vnt")`
dependencies are currently supported. Dependency coordinates, versions,
transitive resolution, remote repositories, and package downloads are not
implemented.

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
properties, task file, compiler executable, every explicitly declared
`implementation` module, and the recursively discovered VNT import graph.
It scans `import "module.vnt"` declarations, resolves each import relative to
its importer before searching configured local repositories, and visits each
source file once to handle repeated and cyclic imports. A changed direct or
transitive imported source invalidates the build even when that module is not
listed in `dependencies {}`. If tracked inputs are current, VBuild prints
`Up to date`. Use `vbuild build --force` to force recompilation.

Native runtime/linker inputs and non-source files are not yet fingerprinted.
Use `--force` after changing those inputs.

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
- Native runtime/linker inputs are not yet fingerprinted. Profiles,
  cross-compilation, remote repositories, versioned/transitive package
  coordinates, arbitrary custom tasks, and reproducible-build guarantees are
  not implemented.
