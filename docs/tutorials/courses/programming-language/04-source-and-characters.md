# 04 — Read Source Text One Character at a Time

## Goal

Write a tiny scanner and learn why its character index must advance.

## Step 1: create the scanner

Create `lesson04.vnt`:

~~~vnt
source = "say \"Hi\""
i = 0
while i < len(source) {
    print(source[i])
    i = i + 1
}
~~~

Compile and run:

~~~powershell
.\vnt.exe --compile lesson04.vnt -o lesson04.exe
.\lesson04.exe
~~~

It prints one character per line: s, a, y, a space, a quote, H, i, and a quote.

## Step 2: print the position too

Replace the loop body with:

~~~vnt
print(i)
print(source[i])
i = i + 1
~~~

There must be exactly one increment each iteration. The position tells you which character the scanner is inspecting.

## Step 3: understand lookahead

Open `vnx/interpreter.vnt` and find the check for `=` followed by another `=`. The lexer examines the next character before emitting the equality token for `==`. That is lookahead.

## Exercise

Change the source value to `"jump door"`. Predict the number of loop iterations, then run it and count the characters.

## Common bug

If you remove `i = i + 1`, the loop repeatedly prints the same character. A lexer must consume input or report an error on every pass.

## Checkpoint

Why is `i < len(source)` important? It prevents reading beyond the end of the string.
