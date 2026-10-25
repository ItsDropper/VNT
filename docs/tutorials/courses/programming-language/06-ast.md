# 06 — Represent Instructions as AST Nodes

## Goal

Use VNT objects to represent an instruction before executing it.

VNX is line-oriented, so its AST is a linked sequence of instruction nodes rather than a large arithmetic-expression tree. The same principle applies: store program structure as data.

## Step 1: type a node

Create `lesson06.vnt`:

~~~vnt
node = object()
node.kind = "SAY"
node.text = "Hello from a node"
node.line = 1

print(node.kind)
print(node.text)
print(node.line)
~~~

Expected output:

~~~text
SAY
Hello from a node
1
~~~

## Step 2: connect another instruction

Add:

~~~vnt
second = object()
second.kind = "END"
second.text = ""
second.line = 2
node.next = second

print(node.next.kind)
~~~

The final line should be `END`.

## Step 3: find the real node builder

In `vnx/interpreter.vnt`, find `make_node`, `add_node`, and `parse`.

- `make_node` creates a node with a kind, source line, payload fields, and next link.
- `add_node` appends it to the program.
- `parse` chooses a node kind based on the command.
- `say "hello"` becomes a `SAY` node; `say player` becomes a `SAYVAR` node.

## Step 4: draw the sequence

For this program:

~~~text
say "First"
say "Second"
end
~~~

Draw three boxes in order. Label them `SAY`, `SAY`, and `END`. Put the text payload in the first two.

## Exercise

Create a node with kind `SET` and text `Ada`. Connect it after your first node and print `node.next.kind`.

## Checkpoint

Source keyword `say`, AST kind `SAY`, and the runtime effect of printing text are three separate things.
