# 06 — Abstract Syntax Trees

Tokens preserve the spelling of the source. An abstract syntax tree (AST) preserves the program's meaningful structure.

For the expression `1 + 2 * 3`, multiplication has higher precedence, so the tree should represent `1 + (2 * 3)`, not `(1 + 2) * 3`.

```text
       ADD
      /   \
   NUMBER  MULTIPLY
    (1)     /   \
         (2)   (3)
```

## Node design

A node can be an object with a `kind` field and fields for its children or payload. A number node needs a number value; a string node needs string content; a binary operator node needs an operator and left/right children.

Statement nodes may represent output, declaration, assignment, conditional, loop, or a block. Keep node kinds explicit rather than trying to infer a node's role from whichever fields happen to be populated.

## AST versus concrete syntax

The AST usually omits details that do not affect execution, such as extra parentheses. It can retain line and column information for diagnostics. You do not need to store every whitespace character.

## Memory and limits

A tree needs a clear ownership strategy. For a first interpreter, use a simple representation and reasonable source/token/node limits. Avoid building a complicated garbage collector before the basic language works.

## Exercise

Draw the tree for a conditional with an output statement in each branch. Label every child pointer or field.

## Checkpoint

Why is a syntax tree a better representation for execution than repeatedly searching the original source text for keywords?
