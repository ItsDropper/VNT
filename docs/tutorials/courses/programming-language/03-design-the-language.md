# 03 — Design the Language Before Coding

## Goal

Write down syntax and behavior so you can tell whether the implementation is correct.

## Step 1: create a specification

Create `my-language-spec.md`:

~~~text
# My language: v0

## Values
All values are strings.

## Commands
say "text"        prints text and a newline.
say name          prints the string stored in name.
set name "text"   stores a string under name.
input name "text" prints a prompt and stores input.
label name        defines a jump destination.
jump name         jumps to a label.
when name == "text" jump label
                  jumps only when name matches text.
end               stops execution.

## Formatting
One command per line.
Blank lines are ignored.
Lines starting with # are comments.
Strings use double quotes.
~~~

This describes VNX v0. Compare it with [the VNX reference](../../../../vnx/README.md). If the docs and implementation disagree, investigate instead of guessing.

## Step 2: write a valid program

Create `hello.vnx`:

~~~text
say "Hello, world!"
set name "Ada"
say name
end
~~~

Run it through the interpreter. Expected output includes `Hello, world!` and `Ada`.

## Step 3: write invalid programs

Try each in a separate file:

~~~text
set name
end
~~~

~~~text
jump missing_label
end
~~~

The first is malformed syntax because `set` requires a name and quoted value. The second has valid command shape but a missing runtime destination.

## Step 4: write the grammar sketch

~~~text
program -> line*
line    -> say | set | input | label | jump | when | end
say     -> "say" STRING | "say" IDENTIFIER
set     -> "set" IDENTIFIER STRING
input   -> "input" IDENTIFIER STRING
label   -> "label" IDENTIFIER
jump    -> "jump" IDENTIFIER
when    -> "when" IDENTIFIER "==" STRING "jump" IDENTIFIER
~~~

This is a design sketch, not a parser generator input.

## Exercise

Invent a command on paper. Write one valid example, one invalid example, its intended behavior, and which layers would need changing. Do not implement it yet.

## Checkpoint

Syntax defines what text is accepted. Semantics defines what accepted text does. For example, syntax says where the value follows `set`; semantics says the variable stores that value.
