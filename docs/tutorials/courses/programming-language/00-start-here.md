# 00 — Start Here

## Your mission

Build a small programming language from scratch, with VNT as the implementation language. You will learn how a program can read another program, understand its structure, and execute it.

There are two languages involved:

- **VNT**: the language used to write the interpreter.
- **Your language**: the language your interpreter reads, such as files ending in `.vnx`.

Do not confuse the implementation with the language it implements. A VNT compiler compiles the interpreter source; the resulting executable then reads your language's source files.

## What you will build

A small interpreted language with text output, variables, input, conditions, and a loop or equivalent control-flow mechanism. Then use it to make a branching text adventure.

You are not trying to beat C, Rust, or Python in this course. You are trying to understand every layer and finish a small implementation that actually runs.

## Vocabulary

- **Source code**: text written by a programmer.
- **Token**: a meaningful unit such as an identifier, keyword, or string literal.
- **Lexer**: turns characters into tokens.
- **Parser**: checks how tokens fit the grammar.
- **AST**: abstract syntax tree; structured data representing the program.
- **Interpreter**: executes the parsed program.
- **Runtime state**: values that exist while a program runs.
- **Diagnostic**: a useful explanation of an error.

## Checkpoint

Explain this sentence in your own words: “VNT runs the interpreter; the interpreter runs the adventure.” If those are still indistinguishable, reread the two-language distinction before coding.
