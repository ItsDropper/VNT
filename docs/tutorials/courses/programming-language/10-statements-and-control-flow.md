# 10 — Statements and Control Flow

## Goal

Write a branching program and trace how the interpreter changes its instruction pointer.

VNX v0 uses `label`, `jump`, and `when` instead of structured `if` and `while` statements.

## Step 1: type a branching program

Create `vnx/tests/branch-demo.vnx`:

~~~text
say "Choose a door: red or blue"
input door "Door? "
when door == "red" jump red_door
when door == "blue" jump blue_door
say "That is not a valid door."
end
label red_door
say "The red door opens."
end
label blue_door
say "The blue door opens."
end
~~~

Run it. Enter `red`; it should print `The red door opens.`. Run it again and enter `blue`. Enter something else to test the invalid-choice path.

## Step 2: understand the instruction pointer

Imagine `pc` points at the current instruction node.

- Normal command: execute it, then advance `pc`.
- `jump red_door`: find the label and set `pc` to the instruction after it.
- `when door == "red" jump red_door`: jump only if the stored value matches.
- `end`: stop the execution loop.

Find label and jump handling in `execute()` in `vnx/interpreter.vnt`.

## Step 3: add an option

Add a third choice, `green`. Add the matching `when` line and a `label green_door` section that prints a message and ends. Test all three routes plus invalid input.

## Checkpoint

A jump can loop forever if it returns to a label without changing state or asking for input. Every loop needs a reason it can eventually stop.
