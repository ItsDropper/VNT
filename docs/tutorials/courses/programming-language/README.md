# Build a Programming Language in VNT — Hands-on Course

This is a practical course. Each lesson tells you **which file to create or open, what to type, how to run it, and what result to expect**. Work through the lessons in order.

You will make small working experiments, inspect the real VNX interpreter, then change a language program and its text adventure. **VNT** is the implementation language. **VNX** is the language being interpreted.

## Before starting

- A checkout of this repository.
- A working VNT compiler.
- A terminal opened at the repository root.
- Basic familiarity with VNT variables, strings, conditions, and functions.

## First working program

Create `lesson.vnt` in the repository root and type:

~~~vnt
print("Hello from the language workshop.")
name = "Ada"
print("Welcome, " + name)
~~~

On Windows PowerShell, compile and run:

~~~powershell
.\build.ps1
.\vnt.exe --compile lesson.vnt -o lesson.exe
.\lesson.exe
~~~

On Linux, after building the compiler with the repository's build process:

~~~sh
./vnt --compile lesson.vnt -o lesson
./lesson
~~~

Expected output:

~~~text
Hello from the language workshop.
Welcome, Ada
~~~

Fix the toolchain first if this example does not run.

## The 15 lessons

| Lesson | What you will do |
|---|---|
| [00 — Start here](00-start-here.md) | Compile your first VNT program |
| [01 — What is a language?](01-what-is-a-language.md) | Run VNT and VNX separately |
| [02 — Prepare VNT](02-prepare-vnt.md) | Compile the real interpreter |
| [03 — Design](03-design-the-language.md) | Write a tiny language specification |
| [04 — Source text](04-source-and-characters.md) | Scan a string by character index |
| [05 — Lexer](05-lexer.md) | Create token objects and link them |
| [06 — AST](06-ast.md) | Represent instructions as nodes |
| [07 — Parser](07-parser.md) | Recognize commands and trace parsing |
| [08 — Values and variables](08-values-and-variables.md) | Track state and variable lookup |
| [09 — Expressions](09-expressions.md) | Understand comparisons and operator rules |
| [10 — Control flow](10-statements-and-control-flow.md) | Make a branching program |
| [11 — I/O and errors](11-input-output-and-errors.md) | Handle input and diagnose invalid code |
| [12 — Text adventure](12-build-a-text-adventure.md) | Build and extend a game |
| [13 — Testing](13-testing-and-debugging.md) | Add repeatable regression tests |
| [14 — Extensions](14-extensions-and-graduation.md) | Plan a feature from syntax to tests |

## Architecture

~~~text
VNX source text
   |
   v
lexer: characters -> tokens
   |
   v
parser: tokens -> AST instruction nodes
   |
   v
interpreter: nodes + runtime state -> behavior
   |
   v
output, input, or an error
~~~

Read the actual implementation alongside the lessons: [vnx/interpreter.vnt](../../../../vnx/interpreter.vnt). The current VNX v0 language supports string values, labels, jumps, conditional jumps, input, output, and termination. Do not assume it supports arithmetic expressions, functions, arrays, or static typing.

## Ground rules

1. Type and run the examples; do not just read them.
2. Change one thing at a time and rerun.
3. Separate VNT compiler errors from VNX lexer, parser, and runtime errors.
4. Every new feature needs an implementation and a test.
5. Documentation is not proof that a feature exists.

## Finish line

You should be able to trace source text into tokens, tokens into nodes, and nodes into runtime behavior; write a new VNX program without changing the interpreter; and test both valid and invalid programs.
