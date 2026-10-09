# VNT

VNT is a small programming language written from scratch in C.

The goal is to build a simple, understandable native language with its own lexer, parser, AST, compiler, and runtime — while keeping the implementation lightweight and maintainable.

> VNT is currently in active development.

## Features

Currently implemented:

* Variables
* Strings
* Integers
* Floating-point numbers
* Booleans
* Strings
* Arrays
* Objects / member access
* Struct definitions and named struct instances
* References and dereferencing (`&x`, `*p`)
* Functions with up to 32 parameters
* Native integer arithmetic fast path with conservative inference for assignment-only variables
* Integer, boolean, and finite floating-point constant folding
* Basic native FFI (`ffi_int`)
* Math functions (`sqrt`, `sin`, `cos`, `tan`, `abs`, `floor`, `ceil`, `min`, `max`)
* Arithmetic expressions
* Operator precedence
* Comparisons
* `if` / `else`
* `print(...)`
* Comments with `#`
* Variable reassignment
* Explicit variable type annotations (`let score: int = 100`)
* Compile-time validation of annotated variable initializers
* Parser source coordinates for common syntax errors
* Runtime error handling
* Nested blocks

### Example

```vnt
let score: int = 100
let username: string = "ItsDropper"
let enabled: bool = true
let accuracy: float = 0.98

print(score)
print(username)
print(enabled)
print(accuracy)
```

Supported annotation names are `int`, `float`, `bool`, `string`, `array`, `object`, and `reference`. An annotated initializer must match its declared type; implicit numeric conversion is not performed for explicit annotations. Existing inferred assignment syntax remains supported.

```vnt
x = 10

if x > 5 {
    print("x is greater than 5")
} else {
    print("x is 5 or less")
}
```

Output:

```text
x is greater than 5
```

## Another Example

```vnt
x = 10
y = 3

print(x + y)
print(x * y)
print(x > y)
print(x == y)
```

Output:

```text
13
30
true
false
```


## Native application API

VNT's generated executables include a cross-platform runtime API:

| Function | Purpose |
| --- | --- |
| `fs_exists(path)` | Test whether a file or directory exists. |
| `fs_read(path)` | Read a text file; a runtime error is raised on failure. |
| `fs_write(path, text)` | Replace file contents; returns a success boolean. |
| `fs_append(path, text)` | Append text; returns a success boolean. |
| `fs_delete(path)` | Delete a file; returns a success boolean. |
| `dir_create(path)` | Create one directory level; returns a success boolean. |
| `cwd()` | Get the current working directory. |
| `env_get(name)` | Read an environment variable; returns `null` if absent. |
| `env_set(name, value)` | Set an environment variable for the current process. |
| `time_ms()` | Read a millisecond clock as a floating-point number. |
| `sleep_ms(milliseconds)` | Block for a non-negative integer duration. |
| `process_start(executable, args)` | Start a child process; `args` is an array of strings. |
| `process_poll(process)` | Return whether the child has exited. |
| `process_wait(process, timeout_ms)` | Wait for exit; `-1` means indefinitely, timeout returns false. |
| `process_pid(process)` | Return the child process ID. |
| `process_terminate(process)` | Request termination; returns whether the OS accepted it. |
| `process_stdout(process)` | Return captured standard output so far. |
| `process_stderr(process)` | Return captured standard error so far. |
| `process_exit_code(process)` | Return exit code, or `-1` while still running. |

File operations currently handle text strings, not arbitrary binary data. These
functions provide the runtime boundary for native applications and process management.
Process launching uses an executable path plus a string argument array; VNT does not
interpret arguments as shell commands. Output is captured separately from standard
output and standard error. A zero timeout polls immediately, positive timeouts are in
milliseconds, and `-1` waits indefinitely. POSIX termination sends SIGTERM; Windows
uses TerminateProcess.

```vnt
settings = "settings.ini"
if !fs_exists(settings) {
    fs_write(settings, "theme=dark\n")
}
print(fs_read(settings))
print(cwd())
print(time_ms())
```

## Real application example: VNT Desk

VNT Desk is a small interactive developer journal demonstrating that VNT can build a
native terminal application rather than only print fixed examples. It supports adding
entries, viewing saved entries, inspecting runtime/environment information, and deleting
the journal with explicit confirmation. Entries are stored as plain text in
`vnt_desk.log` in the program's current working directory; nothing is uploaded.

Build and run it from the repository root:

```powershell
.\vnt.exe --compile examples\vnt_desk.vnt -o examples\vnt_desk.exe
.\examples\vnt_desk.exe
```

The journal path is relative to the working directory, so run the executable from the
directory where you want the data file created. The app uses VNT functions, loops,
conditionals, string handling, and the native file, environment, clock, and current
directory APIs.

## How VNT Works

VNT is compiled to native x86-64 code.

```text
VNT source code
      ↓
    Lexer
      ↓
    Parser
      ↓
     AST
      ↓
 HIR lowering + validation
      ↓
 Constant-folding pass
      ↓
 x86-64 backend adapter
      ↓
   Assembly
      ↓
 GCC + VNT runtime
      ↓
 Native executable
```

The compiler emits x86-64 assembly and links a small native runtime for dynamic values, arrays, strings, objects, and math operations. The HIR retains explicit variable annotations as validated metadata, and its optimizer folds safe integer, boolean, and finite floating-point constant expressions. Source-file imports are currently expanded before parsing; direct circular imports are rejected, but imports do not yet provide isolated namespaces or exported symbols. The old interpreter is no longer part of the build or execution path.

The compiler now lowers the AST into an explicit high-level IR (HIR): a flat, index-addressed node store with typed opcodes, named edge roles, copied literal/declaration metadata, and structural validation. Semantic analysis distinguishes first assignments (variable declarations) from later assignments (reassignments), and HIR represents reassignments with their own opcode. The x86-64 backend still consumes the AST adapter for code generation, so this is an IR representation milestone rather than a completed HIR backend migration. The current x86-64 backend still uses the retained AST source map; migrating code generation to consume HIR directly is the next architectural step.

To inspect the lowered representation:

```powershell
.\vnt.exe --dump-ir tests\native_compiler.vnt
```

## Error Handling

VNT performs basic runtime validation.

For example, using an undefined variable:

```vnt
print(x)
```

produces a runtime error instead of silently continuing.

Division by zero is also detected:

```vnt
x = 10 / 0
```

## Roadmap

* [x] Comments
* [x] Integers
* [x] Floating-point numbers
* [x] Strings
* [x] Booleans
* [x] Arrays
* [x] Objects / member access
* [x] Comparisons
* [x] `if` / `else`
* [x] `while` loops
* [x] Functions
* [x] `return`
* [x] Native x86-64 compiler
* [x] Native integer arithmetic fast path
* [x] Struct definitions
* [x] References / dereferencing
* [x] Basic native library FFI
* [~] Basic source-file imports (currently expanded before parsing; not yet a full module namespace system)
* [~] Native Windows GUI/windowing foundation (basic drawing API; widget toolkit and Linux/macOS backends remain)
* [ ] Game-oriented standard library
* [x] Integer/boolean constant folding with overflow-safe folding rules
* [ ] HIR-native code generation and typed-value optimization

## Design Goals

VNT is being developed around a few principles:

**Simple**

The language should be easy to understand and write.

**Maintainable**

The implementation should remain structured as the language grows.

**Predictable**

Language behavior should be explicit rather than relying on surprising implicit behavior.

**From scratch**

The core language implementation is written in C rather than relying on an existing language runtime.

## Status

VNT is an early-stage experimental programming language.

The syntax, runtime, and internal architecture may change as development continues.

## License

VNT's licensing terms have not been finalized yet.


## Global variables

Top-level variable declarations use shared storage and can be read or updated from functions. Parameters remain local; assignments to a name declared at top level update that global.

Regression example: `examples/global_variables.vnt`.


## Native GUI and graphics (Windows)

VNT supports decimal and hexadecimal integer literals (for example, `42` and `0x2A`). Its native runtime includes a small immediate-mode Win32 GUI API. It is suitable for simple desktop tools and prototypes; it is not yet a full widget toolkit.

- `gui_size(width, height)` — choose window client dimensions before opening.
- `gui_open(title)` — create and show a native window.
- `gui_text(text)` — draw a line of text.
- `gui_fill(0xRRGGBB)` — fill the window background with a color.
- `gui_rect(0xRRGGBB)` — draw a sample rectangle.
- `gui_poll()` — pump window events; returns false after the window closes.
- `gui_key()` — return the next queued virtual-key code, or 0.
- `gui_close()` — close the window.

See `examples/gui_demo.vnt`. GUI support currently targets Windows; Linux/macOS GUI backends are not implemented.
