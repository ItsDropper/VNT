# 05 — Build Token Objects

## Goal

Create one token object, link it to another token, and inspect the real lexer.

## Step 1: type one token

Create `lesson05.vnt`:

~~~vnt
token = object()
token.kind = "STRING"
token.text = "Hello"
token.line = 1

print(token.kind)
print(token.text)
print(token.line)
~~~

Compile and run. Expected output:

~~~text
STRING
Hello
1
~~~

The `object()` function creates a dynamic object. Properties store fields on it.

## Step 2: link a second token

Add these lines to the same file:

~~~vnt
next_token = object()
next_token.kind = "EOF"
next_token.text = ""
next_token.line = 1
token.next = next_token

print(token.next.kind)
~~~

The final line should be `EOF`. A `next` property points to the next token. Repeating this forms a linked list.

## Step 3: inspect the implementation

Open [interpreter.vnt](../../../../vnx/interpreter.vnt) and find `make_token`, `emit`, and `lex`.

- `make_token` creates a token and initializes its fields.
- `emit` appends a token to the list.
- `lex` scans characters and emits tokens.
- The final EOF token gives the parser a clear stopping point.

The actual token fields are `kind`, `text`, `line`, and `next`.

## Step 4: predict the tokens

For the line `say "Hello"`, the lexer emits a command-word token, a string token, a newline token, and eventually EOF. Check the exact token names in the implementation.

## Exercise

Set `token.kind` to `"IDENTIFIER"` and `token.text` to `"say"`. Add a third linked token of kind `STRING` and print `token.next.next.kind`.

## Checkpoint

The lexer describes source structure. It should not perform the output operation itself; that belongs to the runtime.
