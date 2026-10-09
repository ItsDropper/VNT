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
* Native integer arithmetic fast path
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
* [ ] Graphics and windowing
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
