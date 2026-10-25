# Tutorial 3: Arrays, Functions, and Modules

This lesson builds on the first two VNT tutorials. VNT is experimental, so consult the examples and compiler tests if syntax changes.

## 1. Work with arrays

Create `arrays.vnt`:

```vnt
let scores: array = [10, 20, 30, 40]
print(scores[0])
print(scores[2])
```

Array indexing is zero-based: the first element is at index `0`. Do not index outside the array's valid range.

## 2. Loop over values

```vnt
let scores: array = [10, 20, 30, 40]
let index: int = 0
let total: int = 0

while index < 4 {
    total = total + scores[index]
    index = index + 1
}

print(total)
```

Expected output:

```text
100
```

The loop condition prevents reading beyond the four elements. This example uses a fixed length because this language subset does not demonstrate a universal array-length API.

## 3. Put reusable logic in a function

```vnt
fun double(value: int) -> int {
    return value * 2
}

print(double(21))
```

Parameters are local to the function. The return annotation states the value type expected from the function.

## 4. Split code into a module

VNT's module loader supports importing a relative source file and exporting functions. Create `math_helpers.vnt`:

```vnt
export fun square(value: int) -> int {
    return value * value
}
```

Then import it from a source file in the same directory:

```vnt
import "math_helpers.vnt" as math_helpers
print(math_helpers.square(7))
```

Module syntax and member resolution are evolving; use existing module examples in `examples/` as the executable reference for your checkout.

## Exercises

1. Change the array values and calculate their sum.
2. Add a function that returns the larger of two integers.
3. Move that function into a module and import it from another file.

Next: [Native memory and unsafe operations](04-native-memory.md).
