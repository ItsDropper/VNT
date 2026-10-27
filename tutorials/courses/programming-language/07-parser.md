# 07 — Recognize Commands with a Parser

## Goal

Write a tiny command recognizer, then trace how the real parser consumes tokens.

## Step 1: create a recognizer

Create `lesson07.vnt`:

~~~vnt
source = "say"
if source == "say" {
    print("Recognized output command")
} else {
    print("Unknown command")
}
~~~

Compile and run. Expected output:

~~~text
Recognized output command
~~~

Change `source` to `"jump"` and run it again. It should print `Unknown command`.

This is only a warm-up: it compares a whole string, whereas a real parser consumes a sequence of tokens.

## Step 2: inspect parser helpers

In `vnx/interpreter.vnt`, find:

- `take()` returns the current token and advances.
- `expect(kind, description)` checks the token kind and reports a parser error on mismatch.
- `skip_lines()` skips newline tokens.
- `parse()` recognizes statements and appends nodes.

## Step 3: trace one line

For `say "VNX_SMOKE_OK"` in `vnx/tests/smoke.vnx`:

1. Lexer emits the `say` word token.
2. Lexer emits a string token containing `VNX_SMOKE_OK`.
3. Parser recognizes the command.
4. Parser consumes the string and creates a `SAY` node.
5. Runtime later prints the node's stored text.

## Step 4: trigger a parser error

Run `vnx/tests/invalid.vnx` through the interpreter. It intentionally contains `set answer` without a quoted value. The parser should reject it.

## Exercise

Copy the smoke file to `vnx/tests/my-test.vnx`, change the first output string, and run it. Then remove the value from `set` and observe the error.

## Checkpoint

A parser must reject malformed token sequences. Otherwise the runtime receives a program whose meaning is unclear.
