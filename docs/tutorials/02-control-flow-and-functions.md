# Tutorial 2: Conditions, Loops, and Functions

This tutorial combines conditional branches, a `while` loop, and a typed function into a small number-guessing-style score evaluator. It does not read input; the score is set in the source so the example stays focused on control flow.

## 1. Branch with `if` and `else`

Create `control_flow.vnt` in the repository root:

```vnt
score = 72

if score >= 50 {
    print("Passed")
} else {
    print("Try again")
}
```

Only the branch whose condition matches is executed. Comparison expressions such as `>=`, `<`, and `==` produce boolean values.

## 2. Repeat work with `while`

Add this block:

```vnt
count = 0

while count < 3 {
    print(count)
    count = count + 1
}
```

The loop prints `0`, `1`, and `2`. The increment is essential: without changing `count`, the condition would remain true and the loop would not finish.

## 3. Put reusable logic in a function

Add a function before the statements that call it:

```vnt
fun grade(score: int) -> string {
    if score >= 80 {
        return "A"
    } else {
        if score >= 60 {
            return "B"
        } else {
            return "C"
        }
    }
}

print(grade(72))
```

The parameter annotation `score: int` describes the accepted argument type. The `-> string` annotation declares the return type. Each branch returns a string, and a typed non-`void` function must return a value on every path.

## Complete program

```vnt
fun grade(score: int) -> string {
    if score >= 80 {
        return "A"
    } else {
        if score >= 60 {
            return "B"
        } else {
            return "C"
        }
    }
}

score = 72
print(grade(score))

count = 0
while count < 3 {
    print(count)
    count = count + 1
}
```

Compile and run it:

```powershell
.\vnt.exe --compile control_flow.vnt -o control_flow.exe
.\control_flow.exe
```

Expected output:

```text
B
0
1
2
```

## Exercises

1. Change the score to `85` and predict the grade before running it.
2. Change the loop limit from `3` to `5`.
3. Write a typed function `fun is_passing(score: int) -> bool` that returns whether a score is at least 50.

Next: [Modules and text-file storage](03-modules-and-files.md).
