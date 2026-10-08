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
* Math functions (`sqrt`, `sin`, `cos`, `tan`, `abs`, `floor`, `ceil`, `min`, `max`)
* Arithmetic expressions
* Operator precedence
* Comparisons
* `if` / `else`
* `print(...)`
* Comments with `#`
* Variable reassignment
* Runtime error handling
* Nested blocks

### Example

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
 Native x86-64 backend
      ↓
   Assembly
      ↓
 GCC + VNT runtime
      ↓
 Native executable
```

The compiler emits x86-64 assembly and links a small native runtime for dynamic values, arrays, strings, objects, and math operations. The old interpreter is no longer part of the build or execution path.

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
* [ ] C-like native integer fast path
* [ ] Struct definitions
* [ ] Modules / imports
* [ ] Native library FFI
* [ ] Graphics and windowing
* [ ] Game-oriented standard library
* [ ] Compiler optimizations

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
