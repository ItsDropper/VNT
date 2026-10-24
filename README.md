# VNT

VNT is an experimental programming language implemented in C. It compiles VNT source into x86-64 assembly and links a native executable with the VNT runtime.

The project focuses on a small, understandable language, native execution, and a compiler implementation that can evolve without hiding its internals behind a large framework.

- **Implementation:** C
- **Native target:** x86-64
- **Build toolchain:** GCC
- **License:** MIT

> VNT is under active development. Syntax, runtime behavior, and compiler internals may change.

## Contents

- [Quick start](#quick-start)
- [Your first VNT program](#your-first-vnt-program)
- [Language overview](#language-overview)
- [Functions and types](#functions-and-types)
- [Modules and standard library](#modules-and-standard-library)
- [Native runtime APIs](#native-runtime-apis)
- [Examples and tutorials](#examples-and-tutorials)
- [Compiler pipeline](#compiler-pipeline)
- [Project layout](#project-layout)
- [Testing](#testing)
- [Current limitations](#current-limitations)
- [Roadmap](#roadmap)
- [License](#license)

## Quick start

### Requirements

- GCC
- Git, to obtain the repository
- Windows: PowerShell for the supplied build and language-check scripts
- A supported x86-64 environment for the generated native program

Clone the repository and enter its directory:

```powershell
git clone https://github.com/ItsDropper/VNT.git
cd VNT
```

Build the compiler on Windows:

```powershell
.\build.ps1
```

This creates `vnt.exe` in the repository root. Compile a VNT source file into a native executable:

```powershell
.\vnt.exe --compile examples\global_variables.vnt -o global_variables.exe
.\global_variables.exe
```

To inspect the compiler's current high-level intermediate representation (HIR):

```powershell
.\vnt.exe --dump-ir examples\global_variables.vnt
```

The compiler prints the generated assembly path and keeps the assembly file beside the output executable. The native linker step uses GCC and the runtime sources in the repository.

## Your first VNT program

Create a file named `hello.vnt`:

```vnt
# A comment starts with a hash.
let username: string = "VNT"
let score: int = 40 + 2

print("Hello, " + username)
print(score)
```

Compile and run it from the repository root:

```powershell
.\vnt.exe --compile hello.vnt -o hello.exe
.\hello.exe
```

Expected output:

```text
Hello, VNT
42
```

VNT supports inferred assignments such as `score = 42` as well as explicit declarations such as `let score: int = 42`. Supported annotation names include `int`, `float`, `bool`, `string`, `array`, `object`, and `reference`. Explicit annotations are checked against the initializer; VNT does not silently convert a mismatched value.

## Language overview

### Values and expressions

VNT includes integers, floating-point numbers, strings, booleans, arrays, objects/member access, arithmetic, comparisons, boolean expressions, and references/dereferencing.

```vnt
let width: int = 8
let height: int = 5
let area: int = width * height

print(area)
print(width > height)
print(true && !false)
```

Operators follow precedence rules, so multiplication is evaluated before addition. The compiler also performs safe constant folding for supported integer, boolean, and finite floating-point expressions.

### Conditions

```vnt
score = 72

if score >= 50 {
    print("Passed")
} else {
    print("Try again")
}
```

### Loops

`while` repeats a block while its condition is true:

```vnt
count = 0

while count < 5 {
    print(count)
    count = count + 1
}
```

This prints the numbers 0 through 4. Update the condition's variables inside the loop so it eventually stops.

### Functions

```vnt
fun add(a: int, b: int) -> int {
    return a + b
}

fun status(active: bool) -> string {
    if active {
        return "active"
    } else {
        return "inactive"
    }
}

print(add(20, 22))
print(status(true))
```

VNT supports typed parameters and return types, including validation of argument types and return paths for typed non-`void` functions. Unannotated functions remain supported. Parameters are local to the function; top-level variables can be read and updated from functions.

## Modules and standard library

VNT source modules are imported by relative path and expanded by the compiler's module loader. Export functions with `export fun` and call them through the imported module alias:

```vnt
import "../lib/std/math.vnt" as math

print(math.square(6))
print(math.clamp(42, 0, 10))
```

Import paths are resolved relative to the importing source file. The loader rejects malformed imports and direct circular imports. This is an early source-expansion module system, not yet isolated per-module compilation: private symbol isolation and incremental module compilation are not implemented.

The source-level standard library lives in `lib/std/`. Available modules include:

| Module | Purpose |
| --- | --- |
| `io.vnt` | Line input, standard output, and standard error |
| `fs.vnt` | Text files and directory operations |
| `math.vnt` | Math wrappers and helper functions |
| `time.vnt` | Millisecond clock and sleep helpers |
| `os.vnt` | Current directory and environment variables |
| `process.vnt` | Start, inspect, wait for, and terminate child processes |
| `net.vnt` | HTTP helpers backed by the system `curl` executable |
| `log.vnt` | File and console logging |
| `config.vnt` | Plain-text configuration helpers |
| `json.vnt` | JSON-related source helpers |

Example filesystem usage:

```vnt
import "../lib/std/fs.vnt" as fs

path = "notes.txt"
if !fs.exists(path) {
    fs.write_text(path, "First line\n")
}
fs.append_text(path, "Second line\n")
print(fs.read_text(path))
```

File operations handle text, not arbitrary binary data. The networking module requires `curl` (or `curl.exe` on Windows) on `PATH`; it is a runtime bridge to that program, not a built-in HTTP stack. Read the module source and tutorial documentation for exact return behavior and limitations.

## Native runtime APIs

Generated executables link the VNT native runtime. Its built-ins include:

- **Files:** `fs_exists`, `fs_read`, `fs_write`, `fs_append`, `fs_delete`, `dir_create`
- **Environment and paths:** `cwd`, `env_get`, `env_set`
- **Time:** `time_ms`, `sleep_ms`
- **Processes:** `process_start`, `process_poll`, `process_wait`, `process_pid`, `process_terminate`, `process_stdout`, `process_stderr`, `process_exit_code`
- **Math:** `sqrt`, `sin`, `cos`, `tan`, `abs`, `floor`, `ceil`, `min`, `max`

Process arguments are passed as an array of strings rather than interpreted as shell commands. Process output is captured separately for stdout and stderr. Runtime and platform details are still evolving; consult the source-level modules and their implementation for behavior not described here.

## Examples and tutorials

Runnable sample programs are in [`examples/`](examples/). Start with:

- [`global_variables.vnt`](examples/global_variables.vnt) — top-level variables and function access
- [`vnt_desk.vnt`](examples/vnt_desk.vnt) — a terminal journal using file and runtime APIs
- [`vnt_focus.vnt`](examples/vnt_focus.vnt) — a Windows-only focus timer GUI
- [`gui_demo.vnt`](examples/gui_demo.vnt) — basic Windows GUI and stylesheet demo
- [`vanta_one.vnt`](examples/vanta_one.vnt) — larger Vanta GUI example

The GUI runtime is Windows-only. Its CSS-like styling is a small supported subset, not a browser engine or a complete cross-platform widget toolkit.

Follow the [step-by-step coding tutorials](docs/tutorials/README.md), starting with a first program and progressing through control flow, modules and files, a terminal application, and a Windows GUI. The [compiler architecture document](docs/compiler-architecture.md) describes the AST-to-HIR boundary, the HIR-native backend, and the remaining compiler work.

## Compiler pipeline

```text
VNT source
    |
    v
Lexer and parser
    |
    v
AST
    |
    v
HIR lowering, validation, and optimization
    |
    v
x86-64 assembly backend
    |
    v
GCC + native runtime
    |
    v
Native executable
```

The parser produces an AST for front-end semantic analysis and lowering. After lowering, optimization, HIR validation, control-flow analysis, and x86-64 code generation operate on the self-contained, index-addressed HIR. The HIR owns its payloads and stores no pointers into the AST; the native backend traverses HIR nodes directly. The repository includes an AST-free pipeline regression test that destroys the AST before HIR optimization and native code generation. The AST is still used by the frontend and type checker, but it is not a requirement of the HIR backend. See [`docs/compiler-architecture.md`](docs/compiler-architecture.md) for the pipeline and remaining gaps.

## Project layout

```text
VNT/
├── .github/       GitHub workflows
├── docs/          Compiler documentation and tutorials
├── examples/      Runnable VNT sample applications
├── include/vnt/   Public compiler headers
├── lib/std/       Source-level standard modules
├── src/           Compiler, parser, runtime, and backend
├── tests/         Language and compiler regression tests
├── build.ps1      Windows compiler build script
├── LICENSE        MIT license
└── README.md
```

## Testing

On Windows, run the language regression checks from the repository root:

```powershell
.\tests\run_language_checks.ps1
```

The checks build VNT and exercise language features, module loading, and runtime APIs. GitHub Actions also runs focused compiler/language checks. A successful compiler build alone does not prove every generated program behaves correctly, so use the regression suite after changing compiler or runtime code.

## Current limitations

- VNT is experimental; syntax and semantics can change.
- The native backend targets x86-64 and depends on GCC for assembly/linking.
- Frontend parsing and type checking still use the AST before lowering; the HIR optimizer and native backend are designed to work without it.
- Imports expand source before parsing and do not provide private symbol isolation or incremental compilation.
- GUI APIs currently target Windows.
- GUI styling implements a small CSS-like subset, not full CSS or browser behavior.
- Text file APIs are not binary-safe.
- HTTP helpers depend on the system `curl` executable and return response bodies as strings; they are not a native HTTP implementation.
- The standard library and runtime API may change as the language develops.

## Roadmap

- [x] Core expressions, values, variables, and comments
- [x] Conditions and `while` loops
- [x] Functions and return values
- [x] Arrays, objects/member access, structs, and references
- [x] Native x86-64 compiler and runtime
- [x] Initial HIR lowering, validation, and constant folding
- [x] Source-expanded imports and exported functions
- [x] Initial Windows GUI APIs and sample applications
- [x] HIR-native x86-64 code generation and AST-free optimization/codegen path
- [ ] Remove remaining frontend/type-checker AST coupling where it is beneficial
- [ ] Typed-value optimization and broader compiler verification
- [ ] Game-oriented standard library
- [ ] Cross-platform GUI backends

## Design goals

- **Understandable:** keep the language and compiler approachable.
- **Maintainable:** separate frontend, intermediate representation, backend, and runtime responsibilities.
- **Predictable:** validate explicit types and report errors instead of silently accepting invalid behavior.
- **Native:** compile to native code without depending on an embedded interpreter for program execution.
- **From scratch:** implement the core language and compiler in C.

## License

VNT is licensed under the [MIT License](LICENSE). Copyright (c) 2026 ItsDropper.
