# 08 — Runtime Values and Variables

## Goal

Distinguish the variable name written in source from the value stored while a program runs.

## Step 1: experiment with VNT variables

Create `lesson08.vnt`:

~~~vnt
name = "Ada"
score = 7
print(name)
print(score)
score = score + 3
print(score)
~~~

Expected output:

~~~text
Ada
7
10
~~~

These are VNT variables. VNX variables are stored by the interpreter while it runs the VNX program.

## Step 2: trace VNX state

In `vnx/tests/smoke.vnx`:

~~~text
set answer "42"
when answer == "42" jump passed
~~~

When `set` executes, VNX stores the string `42` under the name `answer`. When `when` executes, the runtime looks up that name and compares its stored value with the string.

## Step 3: write a variable program

Create `vnx/tests/variable-demo.vnx`:

~~~text
set hero "Ada"
say hero
set hero "Morgan"
say hero
end
~~~

Run it. It should print `Ada`, then `Morgan`. The second assignment updates the existing value.

## Step 4: inspect the implementation

Find `SET`, `SAYVAR`, and `WHEN` handling in `vnx/interpreter.vnt`. Follow the variable name from the AST node into the runtime environment.

VNX v0 stores strings. `set score "7"` does not create a numeric value; it stores text. Arithmetic is not yet part of VNX v0.

## Exercise

Rename `hero` to `ship_name` everywhere and rerun. Then change only one occurrence and observe why consistent variable names matter.

## Checkpoint

A token spelling `hero` is source text. The runtime value is the string associated with that name.
