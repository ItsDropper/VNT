# Tutorial 6: Arrays, Structs, and References

This lesson moves from basic values to data you can update indirectly. The examples use features present in the repository's current compiler and runtime.

## 1. Arrays are ordered collections

Create `data_structures.vnt` in the repository root:

```vnt
scores = [12, 18, 7, 25]

print(scores[0])
print(scores[2])
print(len(scores))
```

Array indexes start at `0`. For an array with four elements, the valid indexes are `0` through `3`.

## 2. Mutate and process an array

```vnt
scores = [12, 18, 7, 25]
scores[2] = 20

total = 0
i = 0
while i < len(scores) {
    total += scores[i]
    i += 1
}

print(total)
```

The loop uses `len(scores)` rather than hard-coding the number of elements. This makes the same loop work if you change the array's size.

Expected output:

```text
75
```

Try writing a function that accepts an array and returns the sum. If the current type checker does not accept an array parameter annotation, leave the parameter unannotated and focus on the loop.

## 3. Use a struct for related fields

A struct declares a named group of fields. Create a `Vec2` value and update its members:

```vnt
struct Vec2 {
    x,
    y
}

point = Vec2()
point.x = 12
point.y = 30

print(point.x)
print(point.y)
print(point.x + point.y)
```

Expected output:

```text
12
30
42
```

Struct declarations describe the available fields; `Vec2()` creates a value, and member assignment fills its fields. This is useful for coordinates, settings, or other values that naturally belong together. VNT currently has no separate `{ key: value }` object-literal syntax; use a declared struct when you need named fields.

## 4. References: access a variable indirectly

The `&` operator takes a reference to a variable. The `*` operator dereferences it:

```vnt
health = 100
health_ref = &health

print(*health_ref)

*health_ref = 75
print(health)
```

Expected output:

```text
100
75
```

The assignment through `*health_ref` updates the original variable. A reference is not a copy of the integer.

Keep references simple while learning. Only take references to variables whose lifetime and validity you understand; do not assume VNT currently provides Rust-style ownership or borrow checking.

## 5. Combine the concepts

```vnt
struct Player {
    health,
    x,
    y
}

player = Player()
player.health = 100
player.x = 4
player.y = 9

health_ref = &player.health
*health_ref = *health_ref - 15

position_total = player.x + player.y
print(player.health)
print(position_total)
```

Expected output:

```text
85
13
```

This combines structured data, member access, arithmetic, and indirect mutation.

## Exercises

1. Add a fifth score and confirm the sum loop includes it automatically.
2. Write a function that calculates the average of a non-empty array.
3. Add a `level` field to `Player` and print it after assigning a value.
4. Take a reference to `player.x`, update it through the reference, and print the struct member.
5. Explain why `health_ref = health` and `health_ref = &health` are different.

Next: [Native child processes](07-native-processes.md).
