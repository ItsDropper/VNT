# VNX — build a language, then build a game

VNX is a small language implemented as a C tree-walk interpreter. A lexer turns
source into tokens, a parser builds an abstract syntax tree (AST), and the
interpreter evaluates it. The first VNX program is The Last Signal, a branching
text adventure with multiple endings.

This is an interpreter, not a native compiler or self-hosted implementation.
C is the bootstrap; reimplementing the interpreter in VNT/VNX is a future goal.

## Quick start

Requirements: C11 compiler (GCC or Clang) and a POSIX-like shell.

    cd vnx
    make
    ./build/vnx game/last_signal.vnx

On Windows with GCC available:
    gcc -std=c11 -Wall -Wextra -O2 src/vnx.c -o build/vnx.exe
    build/vnx.exe game/last_signal.vnx

The game is interactive. Run regression tests with make test.

## Example

    let player = "Ada";
    let score = 0;
    print("Welcome, " + player);
    while (score < 3) {
        print("Score: " + score);
        score = score + 1;
    }
    if (score == 3) { print("Done!"); }
    else { print("Not done."); }

Statements end with semicolons. Blocks use braces. let declares a variable;
assignment updates an existing variable. Strings support newline, tab,
carriage return, quote, and backslash escapes. Comments start with two slashes.

## Implemented features

- Lexer for identifiers, integers, strings, keywords, comments, operators,
  braces, punctuation, and line-numbered diagnostics.
- AST and recursive-descent parser with arithmetic precedence, comparisons,
  boolean logic, calls, declarations, assignments, blocks, if/else, and while.
- Runtime values: integer, string, boolean, and null.
- Built-ins: print(value) and input(prompt).
- Limits and errors for malformed syntax, unknown names, invalid operators,
  division by zero, oversized source, token/node/variable caps, and runaway code.
- Stateful adventure with inventory, power, dialogue, and multiple endings.

Follow COURSE.md for the complete walkthrough.

## Layout

- src/vnx.c — lexer, AST, parser, value model, evaluator, interpreter.
- game/last_signal.vnx — playable adventure.
- tests/smoke.sh — deterministic regression test.
- COURSE.md — hands-on course.
- Makefile — build and test targets.

## Limitations

No user-defined functions, arrays, objects, modules, garbage collection, static
type checker, bytecode, or native code generation yet. Variables share a global
environment. Built-ins accept one argument. Intermediate strings are not all
reclaimed, so long-running programs can leak memory. Not a sandbox for untrusted
programs. VNX follows the repository MIT license.
