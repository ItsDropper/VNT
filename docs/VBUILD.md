# VBuild

VBuild is VNT's native project build driver. The CLI is implemented in
`tools/vbuild.c` and has no Python runtime dependency. End users run the
prebuilt executable; only building VBuild from source requires a C compiler.

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
.\vbuild init
.\vbuild tasks
.\vbuild build
.\vbuild run
.\vbuild check
.\vbuild clean
```

On Linux use `./vbuild`. Initialization creates `build.vbuild`,
`vbuild.properties`, and `src/main.vnt`. It refuses to overwrite either
configuration file or the starter source.

## build.vbuild task syntax

Each line declares a task name, one built-in action, and optionally dependencies:

```text
# VBuild task file (version 1)
task build = build
task run = run depends build
task check = check
task clean = clean
task test = test
task verify = none depends check, build
```

Grammar:

```text
task NAME = ACTION [depends TASK[, TASK...]]
```

Supported actions are `build`, `run`, `check`, `clean`, `test`, and
`none`. The `none` action lets a task group dependencies without running
another action, as in `verify` above. Run `vbuild verify` to execute the
dependencies in order. A dependency executes at most once per invocation.
Unknown dependencies, duplicate task names, invalid actions, and dependency
cycles are rejected. `vbuild tasks` lists the tasks declared in the file.

This is a deliberately small declarative task graph, not a general-purpose
scripting language. Custom shell commands are not accepted, so a project file
cannot inject arbitrary commands into the VBuild process.

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
`output`, `compiler`, and `toolchain`. `toolchain` is the VNT repository root;
VBuild launches the compiler with that working directory so the compiler can
find its native runtime sources. If omitted, `VNT_HOME` is used when set,
otherwise the current project directory is used. Use an absolute path when the
project lives outside the VNT checkout. `source` points to the single root VNT source
file. `output` must be a file name, and the output is written under `build/`.
VNT's module loader handles imports from that root source.

## Commands and limitations

- `init`: create the starter project.
- `tasks`: list task declarations.
- `build`: compile using the existing VNT compiler.
- `run`: build and execute the output.
- `check`: invoke the compiler's `--dump-ir` parsing/lowering/validation path.
- `clean`: remove generated `build/` output.
- `test`: explicitly reports that test discovery/execution is not implemented.

The VNT compiler and native linker remain prerequisites for compiling programs.
Compiler invocation is now performed without passing the source/output arguments
through a shell, reducing quoting and command-injection risks. VBuild currently
runs the compiler from the current directory, so the compiler's native runtime
source paths must be available relative to that directory. Toolchain-root
discovery, a configurable working directory, dependency resolution, incremental
builds, real test execution, profiles, cross-compilation, and reproducible
build guarantees remain future work.
