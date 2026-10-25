# 01 — What Is a Programming Language?

A programming language is a set of rules for writing instructions and a defined meaning for those instructions. Syntax says what programs look like. Semantics says what they mean.

For example:

```text
say "Hello"
```

A syntax rule might say that `say` must be followed by a string. The semantic rule says that executing this statement writes the string to the output.

## Three different programs

1. The **VNT compiler** reads VNT source and creates a native executable.
2. Your **interpreter**, written in VNT, reads the source of your new language.
3. A **program in your language** tells the interpreter what to do.

The interpreter can be compiled once and then run many different source files. That is why the source program and interpreter should be separate files.

## Interpreter versus compiler

An interpreter executes a program according to its meaning. A compiler translates a program into another representation, such as machine code. Real systems often combine both approaches; these labels describe the main execution strategy, not a strict law.

For this course, choose a tree-walking interpreter: parse the source into nodes, then visit those nodes to execute them. It is approachable because the implementation follows the structure of the language.

## Exercise

Draw three boxes labeled “VNT source”, “interpreter executable”, and “language source”. Add arrows to show which program reads or runs which file.

## Checkpoint

Why is writing a lexer in VNT not the same as writing the lexer in the language you are creating?
