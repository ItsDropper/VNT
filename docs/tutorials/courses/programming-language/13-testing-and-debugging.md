# 13 — Testing and Debugging

A language implementation needs tests for each layer. One successful game run does not prove the lexer, parser, evaluator, or error handling is correct.

## Layered tests

- **Lexer tests**: keywords, identifiers, operators, comments, escapes, EOF, invalid characters.
- **Parser tests**: valid statement forms, precedence, missing delimiters, unexpected tokens.
- **Runtime tests**: arithmetic, variable lookup, assignment, conditions, input/output, invalid operations.
- **Program tests**: complete game paths and ending conditions.
- **Toolchain tests**: build the VNT compiler, compile the VNT interpreter, run the interpreter, and assert expected output.

## Test behavior, not just process exit

A program can exit successfully while producing the wrong answer. Compare expected output, assert important state transitions through visible results, and check that invalid programs fail.

Use deterministic tests for CI. Interactive prompts should be supplied with scripted input when possible, rather than relying on a human to type during automation.

## Regression tests

Whenever you fix a bug, add a test that would have failed before the fix. Otherwise the same bug can return later.

## Debugging sequence

1. Reduce the failure to the smallest source file.
2. Determine which layer owns the bug.
3. Inspect tokens if the issue is lexical.
4. Inspect the AST if the issue is grammatical or precedence-related.
5. Trace runtime state if the structure is correct but the result is wrong.
6. Add a regression test.
7. Run the full suite.

## Exercise

Break one feature on purpose, observe which test catches it, restore it, and add a new edge case.

## Checkpoint

Why is “the program did not crash” a weaker assertion than “the program printed the expected result and returned the expected status”?
