# 11 — Input, Output, and Errors

## Goal

Make the program interact with a user and distinguish syntax errors from runtime errors.

## Step 1: type an input program

Create `vnx/tests/input-demo.vnx`:

~~~text
input player "What is your name? "
say "Hello, technician."
say player
end
~~~

Run it and enter `Ada`. It should print the greeting and then `Ada`. `say "text"` prints a literal; `say player` looks up a variable.

## Step 2: create a parser error

Create `vnx/tests/invalid-demo.vnx`:

~~~text
set player
end
~~~

Run it. The parser should report that `set` needs a variable name and a quoted value.

## Step 3: create a runtime error

Create `vnx/tests/missing-label.vnx`:

~~~text
jump nowhere
end
~~~

Run it. The command has the right form, but the destination does not exist, so the runtime should report an unknown label.

## Four error layers

- **Toolchain error:** VNT cannot compile `interpreter.vnt`.
- **Lexical error:** source contains a character or string the lexer cannot handle.
- **Parser error:** tokens do not match the required command form.
- **Runtime error:** a valid command refers to missing state, such as a label.

## Exercise

Copy a failing example and fix it. Then make another copy with a different error. Keep one error per file so you know which layer rejected it.

## Checkpoint

A useful diagnostic explains what went wrong and where. “Expected a quoted value after set player” is more actionable than “program failed”.
