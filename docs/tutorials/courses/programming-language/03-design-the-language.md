# 03 — Design the Language Before Coding

A language becomes difficult when its rules are invented randomly during implementation. Write a small specification first.

## Choose a tiny first version

A beginner-friendly first version needs only a few features:

- output text;
- variables;
- input;
- conditional branching;
- a way to repeat or jump between sections;
- comments;
- clear syntax errors.

You can add arithmetic, functions, arrays, and types later. Do not add all of them at once.

## Write examples first

Before implementing a feature, write a valid example of how a user should express it. For example, decide whether statements end with semicolons or newlines, how strings are quoted, and how comments begin.

Then write an invalid example. If you cannot say why it is invalid, the rule is probably underspecified.

## Syntax versus semantics

Syntax: “A variable declaration starts with the keyword `let`.”

Semantics: “When the declaration runs, the variable is created and receives the evaluated initial value.”

Do not make the parser responsible for runtime behavior. The parser should describe the structure; the interpreter should give it meaning.

## Grammar sketch

A grammar is a compact description of valid forms. A tiny language might have rules resembling:

```text
program     -> statement*
statement   -> output | declaration | assignment | conditional | loop
output      -> "say" STRING
declaration -> "let" IDENTIFIER "=" expression
```

This is illustrative notation, not VNT syntax. The exact grammar must match the syntax your implementation actually accepts.

## Exercise

Create a one-page specification with examples of every supported statement, expression, string escape, and comment. Add at least three invalid examples and their expected errors.

## Checkpoint

Can another beginner write a valid program using only your specification, without reading the interpreter source?
