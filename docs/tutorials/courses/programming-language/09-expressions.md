# 09 — Expressions, Comparisons, and Operator Rules

## Goal

Understand expression evaluation and the importance of documenting only operators that really work.

VNT supports arithmetic, but **VNX v0 does not support general arithmetic expressions**. Use VNT experiments to learn evaluation and VNX to test its supported string comparison.

## Step 1: run arithmetic in VNT

Create `lesson09.vnt`:

~~~vnt
a = 10
b = 3
print(a + b)
print(a - b)
print(a * b)
print("Ada" + " Lovelace")
~~~

Expected output:

~~~text
13
7
30
Ada Lovelace
~~~

## Step 2: test VNX equality

Create `vnx/tests/compare-demo.vnx`:

~~~text
set answer "42"
when answer == "42" jump matched
say "Comparison failed"
end
label matched
say "Comparison matched"
end
~~~

Run it with the interpreter. Expected output includes `Comparison matched`.

## What an evaluator must define

Before implementing an operator, decide:
1. Which value types it accepts.
2. What result type it returns.
3. What happens when operand types are invalid.
4. Where precedence is enforced.
5. Which error appears for invalid input.

For example, `1 + 2 * 3` usually means `1 + (2 * 3)`. The parser must build that grouping correctly.

## Exercise

Change the comparison to `answer == "7"`. It should take the failure route. Then change the stored value to `7` and confirm the matching route works.

## Important limitation

Do not expect `say 1 + 2` to calculate in VNX. VNX v0 supports `say "text"` or `say variable`, and `when` compares a variable to a string. Arithmetic requires changes to the lexer, parser, AST, evaluator, tests, and reference.
