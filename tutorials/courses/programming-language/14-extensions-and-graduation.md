# 14 — Extend the Language and Graduate

## Goal

Plan a feature from syntax to tests instead of randomly editing the parser.

## Step 1: write a feature proposal

Create `feature-plan.md`:

~~~text
# Proposed feature: repeat

## Example syntax
repeat 3
say "Hello"
endrepeat

## Intended behavior
Print Hello three times.

## Implementation areas
- lexer: recognize repeat and endrepeat
- parser: represent a repeat block
- AST: store the count and body
- runtime: execute the body the requested number of times
- errors: reject invalid counts
- tests: check 0, 1, and 3 repetitions
- docs: update the language reference
~~~

This is a design exercise only. VNX v0 does not support `repeat` or `endrepeat`.

## Step 2: write tests before coding

Specify expected output for counts 0, 1, and 3, plus a malformed count and a missing closing command. Decide the behavior before implementing it.

## Step 3: trace the implementation path

For a real VNX extension, inspect these functions in `vnx/interpreter.vnt`:

- `lex` and `emit`: source to tokens.
- `parse` and `make_node`: tokens to instructions.
- `execute`: instructions to runtime behavior.
- Error handling: malformed programs must be rejected.

Then update `vnx/README.md`, add tests under `vnx/tests/`, update `.github/workflows/vnx-tests.yml`, and run CI.

## Graduation project

Build a game or interactive tool that prints instructions, reads input, stores at least two state variables, branches on prior choices, has two reachable endings, handles invalid choices, and has a scripted test for each ending.

## Final checklist

- [ ] I can compile and run VNT.
- [ ] I can compile the VNT-native interpreter.
- [ ] I can explain tokens, nodes, runtime state, and the instruction pointer.
- [ ] I can write VNX without changing the interpreter.
- [ ] I can diagnose invalid syntax and a missing label.
- [ ] I can plan a feature and test it before implementation.
- [ ] The docs distinguish implemented features from planned ones.

A small language you understand is more valuable than a large language with imaginary features.
