# VBuild

VBuild is VNT's native project build driver. The command-line implementation is
C in `tools/vbuild.c`; it does not require Python or a Python runtime. Build
the tool itself with a C compiler, then distribute the resulting executable as
`vbuild.exe` on Windows or `vbuild` on Linux.

## Build the VBuild executable

From the VNT repository root:

```sh
gcc -std=c11 -O2 tools/vbuild.c -o vbuild
```

On Windows with GCC:

```powershell
gcc -std=c11 -O2 tools/vbuild.c -o vbuild.exe
```

Put the executable in the VNT project directory (or on PATH). VBuild currently
expects the VNT compiler executable to be available as `vnt` on PATH, or to be
named explicitly using the `compiler` property.

## Create and build a project

```powershell
.\vbuild init
.\vbuild build
.\vbuild run
.\vbuild check
.\vbuild clean
```

On Linux use `./vbuild init`, etc. `init` creates `build.vbuild`,
`vbuild.properties`, and `src/main.vnt`. Existing manifests are never
overwritten.

## build.vbuild

The initial build script is deliberately declarative and limited to the
built-in task graph:

```text
# VBuild task file (version 1)
task build
task run depends build
task check
task clean
task test
```

VBuild validates the declared task set before running a command. Arbitrary
build-script code and custom task actions are not supported yet; this is a
small first version, not a general-purpose scripting engine.

## vbuild.properties

```properties
project.name = hello
project.version = 0.1.0
source = src/main.vnt
output = hello
compiler = vnt
```

Supported properties are `project.name`, `project.version`, `source`,
`output`, and `compiler`. The current MVP builds one root source file;
VNT's existing import/module loader remains responsible for module loading.
The output file is written to `build/`.

## Commands and current limits

- `init`: generate starter files without overwriting existing manifests.
- `build`: compile the configured source using VNT's existing compiler.
- `run`: build and execute the host binary.
- `check`: invoke the compiler's `--dump-ir` validation path.
- `clean`: remove the generated `build/` directory only.
- `test`: reports that test discovery/execution is not implemented. It does
  not claim to run tests.

The compiler and native linker remain prerequisites for compiling VNT programs.
The build driver itself requires a C compiler only when building VBuild from
source; end users can run the prebuilt native executable without Python.

Dependency resolution, incremental builds, test discovery, custom task actions,
profiles, cross-compilation, and reproducible build guarantees are not
implemented. The current compiler invocation still relies on host toolchain
availability. Later work should replace shell-based process invocation with
platform-specific process APIs, improve executable discovery, and add native
integration tests on Windows and Linux.
