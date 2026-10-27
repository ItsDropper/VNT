# Tutorial 1: Your First VNT Program

In this tutorial, you'll create a small program, store values in variables, perform an expression, and print results.

## 1. Create a source file

Create `hello.vnt` in the repository root:

```vnt
# Comments begin with a hash.
let language: string = "VNT"
let answer: int = 40 + 2
let ready: bool = true
let ratio: float = 0.5

print("Hello from " + language)
print(answer)
print(ready)
print(ratio)
```

The `let name: type = value` form declares a variable with an explicit type. VNT also supports inferred assignment such as `answer = 42`. Explicit annotations are checked against their initial values; a mismatched type is an error rather than an automatic conversion.

## 2. Compile it

Run these commands from the repository root:

```powershell
.\vnt.exe --compile hello.vnt -o hello.exe
.\hello.exe
```

Expected output:

```text
Hello from VNT
42
true
0.5
```

VNT compiles source to x86-64 assembly and links a native executable with its runtime. This is not an interpreted script invocation.

## 3. Change a value

Add these lines to the end of the program:

```vnt
answer = answer + 1
print(answer)
```

Recompile and run it. The final line should now print `43`. Reassignment updates an existing variable; it does not require another `let`.

## 4. Try an expression

Replace the assignment with:

```vnt
answer = 2 + 3 * 4
print(answer)
```

Multiplication has higher precedence than addition, so the result is `14`.

## What you learned

- `.vnt` files contain VNT source code.
- `print(...)` writes a value to standard output.
- Variables can use explicit type annotations or inferred assignment.
- Arithmetic expressions follow operator precedence.
- Compile the source again after making changes to produce a new executable.

Next: [Conditions, loops, and functions](02-control-flow-and-functions.md).
