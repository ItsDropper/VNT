# 08 — Runtime Values and Variables

Parsing a declaration does not create the variable's runtime value. That happens when the interpreter executes the declaration node.

A runtime environment maps names to values. Conceptually, `score = 7` creates or updates a binding from the name `score` to the integer value `7`.

## Value representation

A simple dynamically typed language might have values for integers, booleans, strings, and a null-like value. Each value should have an explicit type tag. Do not guess whether a value is a string by inspecting its contents.

An object can represent a value record, but make sure each operation checks the tag before reading the corresponding payload. For example, addition between integers can mean numeric addition, while addition between strings may mean concatenation if the specification says so.

## Lookup and assignment

Variable lookup should search the environment for a matching name. If no binding exists, report an “unknown variable” error. Assignment should update the intended binding and should not accidentally create misspelled variables unless that is explicitly the language's rule.

Decide whether blocks create local scopes. A first language can use one global environment, but document that limitation. Adding block scope later changes semantics and requires tests.

## Exercise

Trace this program by hand:

```text
let score = 4
score = score + 3
say score
```

Write down the environment after each statement.

## Checkpoint

What is the difference between the identifier token `score` and the runtime value stored under that name?
