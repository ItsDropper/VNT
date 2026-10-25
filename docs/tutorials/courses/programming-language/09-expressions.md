# 09 — Evaluate Expressions

An expression produces a value. An interpreter evaluates the AST recursively: a literal returns its value, a variable reads its binding, and a binary operator evaluates its operands and applies the operator.

## Operator semantics

Define each operator before implementing it. Decide whether `+` supports numbers, strings, or both. Decide whether comparisons require matching types. Decide what division by zero does. Do not let accidental host-language behavior define your language.

## Precedence and associativity

Precedence determines which operator groups first. Associativity determines how operators of equal precedence group. For example, subtraction is commonly left-associative: `10 - 3 - 2` means `(10 - 3) - 2`.

A precedence-based parser naturally builds a tree with the intended grouping. Parentheses let the programmer override the default grouping.

## Boolean logic

Conditions should evaluate to a defined boolean value, or the language must clearly define truthiness. Avoid silently treating arbitrary strings or numbers as booleans unless that is a deliberate design choice.

Short-circuit behavior is also a semantic decision: with `false && rhs`, should `rhs` be evaluated? If the language uses conventional short-circuit logic, it should not be evaluated when the left side already determines the result.

## Runtime safety

Check invalid operand types and division by zero. Report the source location of the operator where possible. Do not rely on undefined behavior or crashes as an error-handling strategy.

## Exercise

For each expression in your language reference, write at least one expected result and one invalid operand combination.

## Checkpoint

If the lexer recognizes `+` but the evaluator does not implement it, is addition a supported language feature? Explain.
