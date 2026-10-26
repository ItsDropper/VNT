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
- [API reference](docs/api-reference.md)
- [Raw pointers and memory](docs/raw-memory.md)
- [VNT programming courses and tutorials](docs/tutorials/README.md)
- [Complete beginner VNTOS operating-system course (19 lessons)](docs/tutorials/courses/os/README.md)
- [Build a programming language in VNT (15 beginner lessons)](docs/tutorials/courses/programming-language/README.md)
- [VNTOS kernel walkthrough](docs/tutorials/05-vntos-kernel-walkthrough.md)
- [Advanced low-level systems programming course](docs/tutorials/10-low-level-systems-programming.md)
- [Build and try the VNTOS bootable kernel](docs/tutorials/vntos-beginners.md)
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

### Experimental freestanding assembly mode

A separate, deliberately restricted mode can emit an ELF-oriented x86-64 entry object without linking the hosted VNT runtime:

```bash
./vnt --build-freestanding tests/freestanding_minimal.vnt -o kernel.elf
```

For debugging the generated source separately, use `--emit-freestanding-asm`. The build command assembles without hosted startup/runtime objects and links with `ld -nostdlib -static` using [the x86-64 linker script](freestanding/x86_64.ld).

The initial freestanding subset requires a zero-argument `fun kernel_main()`. It supports 32-bit `int`/`bool` values, local variables, arithmetic/comparisons, `if`, `while`, loop control, literal/ordinary integer returns, and inline assembly (including nested `unsafe` blocks). Signed arithmetic overflow traps with `ud2`. Hosted operations such as `print`, strings/floats, pointers, memory builtins, and function calls are rejected rather than linked against the runtime. The linker script emits a static ELF64 image with a private stack and `_start` entry. The repository also contains a separate bootable learning kernel in [`os/`](os/README.md), with a Multiboot2 bootstrap and a serial shell; see the [beginner tutorial](docs/tutorials/vntos-beginners.md). The freestanding backend remains deliberately restricted and rejects hosted features it cannot support.

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

VNT supports inferred assignments such as `score = 42` as well as explicit declarations such as `let score: int = 42`. Supported annotation names include `int`, `float`, `bool`, `string`, `array`, `object`, `reference`, and `ptr`. Explicit annotations are checked against the initializer; VNT does not silently convert a mismatched value.

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
| `array.vnt` | Search/count, numeric aggregates, reverse, concatenate, take/drop, stable deduplication, and join |
| `string.vnt` | Prefix/suffix/search/count/repeat, replacement, and ASCII whitespace trimming |
| `number.vnt` | Parity, sign, factorial, GCD, and primality helpers |
| `time.vnt` | Millisecond clock and sleep helpers |
| `os.vnt` | Current directory and environment variables |
| `process.vnt` | Start, inspect, wait for, and terminate child processes |
| `net.vnt` | Native HTTP/HTTPS requests, status codes, headers, and downloads |
| `log.vnt` | File and console logging |
| `config.vnt` | Load/save files and read INI-style sections, keys, quoted values, and booleans |
| `json.vnt` | JSON-related source helpers |
| `csv.vnt` | CSV parsing/serialization with quoted commas, escaped quotes, and multiline fields |
| `path.vnt` | Portable separators, basename, dirname, extension, stem, and path joining |
| `mem.vnt` | Raw pointer allocation, offset, read/write, copy, and fill primitives; see [raw-memory limits](docs/raw-memory.md) |

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

File operations handle text, not arbitrary binary data. Networking is implemented in the native C runtime: Windows uses WinHTTP, while Unix-like builds use sockets and OpenSSL. No `curl` executable is required. Use `net.request(method, url, headers, body)` for custom methods and headers. HTTP responses are returned as strings; status helpers return a three-digit status string and `000` indicates a transport failure. Downloads preserve binary bytes, write successful 2xx responses to disk, and are the preferred API for large files. Direct download responses stream in bounded chunks; some redirect/proxy fallback paths may buffer the response. Ordinary request APIs return the complete response in memory and are therefore unsuitable for arbitrarily large bodies. See the [networking tutorial](docs/tutorials/08-http-apis.md) and [API reference](docs/api-reference.md) for details.

## Native runtime APIs

Generated executables link the VNT native runtime. Its built-ins include:

- **Files:** `fs_exists`, `fs_read`, `fs_write`, `fs_append`, `fs_delete`, `dir_create`
- **Environment and paths:** `cwd`, `env_get`, `env_set`
- **Time:** `time_ms`, `sleep_ms`
- **Processes:** `process_start`, `process_poll`, `process_wait`, `process_pid`, `process_terminate`, `process_stdout`, `process_stderr`, `process_exit_code`
- **Math:** `sqrt`, `sin`, `cos`, `tan`, `abs`, `floor`, `ceil`, `min`, `max`
- **Raw memory:** `mem_alloc`, `mem_free`, `mem_is_null`, `mem_realloc`, `mem_ptr_add`, `mem_read_u8`, `mem_read_u32`, `mem_write_u8`, `mem_write_u32`, `mem_copy`, `mem_set`

Process arguments are passed as an array of strings rather than interpreted as shell commands. Process output is captured separately for stdout and stderr. Runtime and platform details are still evolving; consult the source-level modules and [API reference](docs/api-reference.md) for behavior not described here.

## API reference

Use the [VNT API reference](docs/api-reference.md) for the standard-library modules, runtime built-ins, return-value conventions, platform requirements, and known limitations. It links directly to the implementation in `lib/std/` and identifies areas where behavior is still experimental.

## Examples and tutorials

The tutorial series covers language basics through a complete task tracker. For systems-level details, see the [advanced low-level systems programming course](docs/tutorials/10-low-level-systems-programming.md), which labels supported features and future work explicitly.

Runnable sample programs are in [`examples/`](examples/). Start with:

- [`global_variables.vnt`](examples/global_variables.vnt) — top-level variables and function access
- [`vnt_desk.vnt`](examples/vnt_desk.vnt) — a terminal journal using file and runtime APIs
- [`vnt_focus.vnt`](examples/vnt_focus.vnt) — a Windows-only focus timer GUI
- [`gui_demo.vnt`](examples/gui_demo.vnt) — basic Windows GUI and stylesheet demo
- [`vanta_one.vnt`](examples/vanta_one.vnt) — larger Vanta GUI example
- [`task_tracker.vnt`](examples/task_tracker.vnt) — complete local task tracker with persistence and checked file-operation results

The GUI runtime is Windows-only. Its CSS-like styling is a small supported subset, not a browser engine or a complete cross-platform widget toolkit.

Follow the [step-by-step coding tutorials](docs/tutorials/README.md), progressing through control flow, modules and files, arrays/structs/references, child processes, networking, and a complete local task-tracker project. The tutorial index also links to the Windows GUI lesson. For runnable source, browse [`examples/`](examples/); for language/runtime contracts, use the [API reference](docs/api-reference.md) and [raw-memory guide](docs/raw-memory.md). The [compiler architecture document](docs/compiler-architecture.md) describes the AST-to-HIR boundary, the HIR-native backend, and the remaining compiler work.

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
- The hosted native backend targets x86-64 and depends on GCC for assembly/linking. An experimental ELF-oriented freestanding backend exists for a restricted `kernel_main` subset; hosted features are rejected when unsupported. A separate bootable x86-64 learning kernel is maintained under `os/`.
- Frontend parsing and type checking still use the AST before lowering; the HIR optimizer and native backend are designed to work without it.
- Imports expand source before parsing and do not provide private symbol isolation or incremental compilation.
- GUI APIs currently target Windows.
- GUI styling implements a small CSS-like subset, not full CSS or browser behavior.
- Text file APIs are not binary-safe.
- Native HTTP/HTTPS support is still evolving. Ordinary response values are text-oriented; streaming downloads write directly to files for direct requests, while redirect/proxy download fallbacks are buffered. The Unix transport supports HTTP(S) proxy environment variables and NO_PROXY; Windows uses WinHTTP's system proxy behavior. Binary-safe request/response arrays are available, but a compact dedicated byte-buffer type and Unix TLS connection pooling remain unimplemented. Unix direct HTTP requests reuse a single keep-alive connection when response framing permits; Windows uses WinHTTP's shared connection pool. The Unix-like HTTPS path requires OpenSSL development/runtime libraries.
- Raw `ptr` values and memory primitives are an initial low-level foundation. Direct raw memory builtins and calls to `unsafe fun` require an `unsafe { ... }` block. Bounds and lifetime remain the caller's responsibility; sizes/offsets are currently signed 32-bit integers and `u64` access is not available. See [raw-memory documentation](docs/raw-memory.md).
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
- [x] Raw pointer type and explicit native memory primitives
- [x] `unsafe { ... }` blocks, `unsafe fun` declarations, and call-site validation
- [ ] `usize`/`isize` and full-width integer operations
- [ ] Operand-aware inline assembly, register constraints, and memory clobbers
- [ ] Freestanding compilation/linking and OS-development target
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


## Freestanding x86-64 and VNTOS

VNT can emit a freestanding x86-64 ELF kernel without linking the hosted runtime.
A minimal bootable operating-system example with a Multiboot2 long-mode bootstrap,
COM1 serial output, and an interactive serial command shell lives in
[os/](os/README.md). See the [beginner tutorial](docs/tutorials/vntos-beginners.md).

On Debian/Ubuntu/WSL, install `build-essential binutils grub-pc-bin grub-common
xorriso mtools qemu-system-x86`, then run:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

The native x86-64 runtime now stores signed integers as 64-bit values, with
overflow checks on signed addition, subtraction, multiplication, division, and
negation. The freestanding backend uses 64-bit integer arithmetic as well.
Unsigned integer types and complete unsigned arithmetic are still future work.
