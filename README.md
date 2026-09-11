# VNT

VNT is a small programming language written from scratch in C.

The goal is to build a simple, understandable language with its own lexer, parser, AST, interpreter, runtime values, and environment — while keeping the implementation lightweight and maintainable.

> VNT is currently in active development.

## Features

Currently implemented:

* Variables
* Strings
* Integers
* Booleans
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

VNT uses a traditional interpreter pipeline:

```text
VNT source code
      ↓
    Lexer
      ↓
    Parser
      ↓
     AST
      ↓
  Interpreter
      ↓
Environment / Values
      ↓
    Output
```

### Lexer

The lexer converts source code into tokens such as identifiers, integers, operators, parentheses, and braces.

### Parser

The parser takes those tokens and builds an Abstract Syntax Tree (AST).

VNT uses operator precedence so expressions such as:

```vnt
x = 2 + 3 * 4
```

are interpreted correctly.

### AST

The AST represents the structure of the VNT program independently from the original source text.

### Interpreter

The interpreter walks the AST and executes the program.

### Environment

The environment stores variables and their current values.

## Project Structure

```text
VNT/
├── src/
│   ├── main.c
│   ├── lexer.c
│   ├── lexer.h
│   ├── parser.c
│   ├── parser.h
│   ├── ast.c
│   ├── ast.h
│   ├── interpreter.c
│   ├── interpreter.h
│   ├── environment.c
│   ├── environment.h
│   ├── value.c
│   └── value.h
│
├── tests/
│   └── hello.vnt
│
└── README.md
```

## Building

VNT currently uses GCC.

From the project directory:

```powershell
gcc src/main.c src/lexer.c src/parser.c src/ast.c src/interpreter.c src/environment.c src/value.c -o vnt.exe
```

Then run a VNT program:

```powershell
.\vnt.exe tests\hello.vnt
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

The language is being developed incrementally.

* [x] Comments
* [x] Integers
* [x] Comparisons
* [x] Booleans
* [x] `if` / `else`
* [x] Arithmetic
* [x] `while` loops
* [x] Functions
* [x] `return`
* [x] Arrays / lists
* [ ] Additional types and language features

The roadmap is intentionally kept small. New features should build on the existing language architecture rather than turning VNT into an unnecessarily complicated language.

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
