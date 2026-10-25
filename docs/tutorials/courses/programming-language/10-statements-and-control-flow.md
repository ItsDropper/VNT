# 10 — Statements and Control Flow

Statements perform actions. Expressions produce values. Keeping this distinction clear makes the interpreter easier to reason about.

An output statement evaluates its expression and writes the result. A declaration evaluates an initializer and stores a value. A conditional evaluates its condition and executes exactly one branch. A loop repeatedly evaluates its condition and executes its body while the condition remains true.

## Blocks

A block is an ordered list of statements. Execute its children in source order. If a runtime error occurs, stop or propagate it according to your documented error policy.

## Conditions

The condition's runtime value must follow the language's boolean rules. Do not accidentally treat a missing variable as false; an unknown name should normally be a diagnostic.

## Loops and runaway code

An accidental infinite loop can hang tests. A step counter or instruction budget is a useful first safety measure for a learning interpreter. It is not a full sandbox and does not make untrusted programs safe.

## Alternative control-flow design

A very small language can use labels and jumps instead of structured loops, but then it must define what happens when a label is missing or a jump repeats forever. Structured `if` and `while` are more familiar to many beginners.

## Exercise

Write a countdown and a condition that selects between two outputs. Test both branches. If your language supports loops, also test a loop that executes zero times and one that executes several times.

## Checkpoint

What makes a loop terminate, and how can your interpreter protect tests from a mistaken infinite loop?
