# Build a Programming Language in VNT

A complete beginner course on building a small interpreted language **using VNT as the implementation language**. You do not need prior compiler knowledge. You should be able to read basic variables, conditions, loops, functions, and strings in VNT.

> Important distinction: VNT is the implementation language for the interpreter. The language you create is the language being interpreted. A file written in your new language is not a `.vnt` file.

## Learning path

| Lesson | Topic | Result |
|---|---|---|
| [00](00-start-here.md) | Start here | Understand the project and finish line |
| [01](01-what-is-a-language.md) | What is a programming language? | Separate source, interpreter, and program |
| [02](02-prepare-vnt.md) | Prepare your tools | Build and run VNT programs |
| [03](03-design-the-language.md) | Design before coding | Write a tiny language specification |
| [04](04-source-and-characters.md) | Read source text | Work with strings and character positions |
| [05](05-lexer.md) | Lexer and tokens | Turn characters into meaningful tokens |
| [06](06-ast.md) | AST and nodes | Represent program structure as data |
| [07](07-parser.md) | Parser | Turn tokens into a syntax tree |
| [08](08-values-and-variables.md) | Values and variables | Store and retrieve runtime values |
| [09](09-expressions.md) | Evaluate expressions | Implement operators and precedence |
| [10](10-statements-and-control-flow.md) | Statements and control flow | Run blocks, conditions, and loops |
| [11](11-input-output-and-errors.md) | I/O and diagnostics | Build usable programs and readable errors |
| [12](12-build-a-text-adventure.md) | Text adventure | Make a complete interactive game |
| [13](13-testing-and-debugging.md) | Testing | Prove language behavior with regression tests |
| [14](14-extensions-and-graduation.md) | Extensions | Plan functions, arrays, and future features |

## The architecture

```text
source file
    |
    v
lexer -- characters become tokens
    |
    v
parser -- tokens become an AST
    |
    v
interpreter -- AST nodes execute against runtime state
    |
    v
output / input / program result
```

The course is intentionally incremental. Do not begin with a large language, bytecode VM, optimizer, or native compiler. First make a tiny language whose behavior you can explain and test.

## Ground rules

- Implement the interpreter in **VNT source**. Do not silently replace it with a C, Python, or Java implementation.
- Keep the example language separate from VNT source files.
- Each feature needs a syntax rule, implementation, tests, and documentation.
- Make one small change at a time and run the regression suite.
- Never call a feature implemented merely because a README describes it.

## Prerequisites

You need basic familiarity with VNT variables, strings, `if`, `while`, and functions. If those are unfamiliar, read the introductory lessons linked from [the tutorial index](../../README.md) first.

## Completion checklist

- [ ] A VNT program reads a source file for the language.
- [ ] A lexer emits tokens and useful locations.
- [ ] A parser constructs a representation of program structure.
- [ ] An interpreter executes the parsed program.
- [ ] Variables and control flow work.
- [ ] The text adventure runs in the created language.
- [ ] Tests cover valid programs and invalid syntax.
- [ ] The language reference documents supported syntax and limitations.
