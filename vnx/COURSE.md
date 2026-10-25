# VNX Course: from characters to a playable language

Make each change, rebuild, and test before moving on.

## 1 — Run the language
Run make inside vnx, then run the interpreter on game/last_signal.vnx. Explore
Security, restore power in the Generator Wing, and try the airlock. Run make test.
Checkpoint: explain why the CPU cannot execute VNX source directly.

## 2 — Lexing
Inspect lex(), emit(), and keyword() in src/vnx.c. For let count = 12 + 3;,
the lexer emits LET, ID, EQ, NUM, PLUS, NUM, SEMI, EOF.
Tasks: print tokens temporarily; test comments and escaped strings; explain why
= and == differ; try an unterminated string.
Checkpoint: add a keyword and trace all places that must change.

## 3 — AST design
Find NodeKind, struct Node, and make(). For 1 + 2 * 3, addition is the root and
multiplication its right child. Draw trees for print("hello"); and an if/else.
Add a temporary AST dumper.
Checkpoint: explain how the tree represents precedence.

## 4 — Recursive-descent parser
Trace statement parsing for declarations, assignments, conditionals, loops,
and blocks. Trace expressions through expr, land, equality, compare, term,
factor, unary, and primary.
Tasks: predict print(1 + 2 * 3);; break a parenthesis; add remainder (%) through
lexer, parser, runtime, tests, and docs.
Checkpoint: a feature is incomplete until every layer agrees.

## 5 — Values and variables
Inspect Value, eval(), lookup(), and assign(). Runtime values include null,
integer, string, and boolean. Bindings map names to values.
Tasks: trace let x = 4; x = x + 1; print(x);; reassign a string; design type().
Checkpoint: distinguish a variable name from its stored value.

## 6 — Control flow
if executes one branch; while repeats until its condition is false. A step limit
guards against accidental infinite loops.
Tasks: write a countdown; write an input loop ending at quit; add a flag that
changes a later scene.
Checkpoint: explain why runtime errors stop loop execution.

## 7 — Built-ins and input
Built-ins are handled by CALL in eval(). print(value) writes a value and
input(prompt) reads one line as a string.
Tasks: design integer conversion with explicit invalid-input behavior; decide
what end-of-file means; improve unknown-built-in diagnostics.
Checkpoint: the parser recognizes calls; the runtime implements names.

## 8 — Read the adventure
Open game/last_signal.vnx. location selects rooms; has_key, power_on, has_sample,
and trust_ai preserve decisions.
Tasks: add a room, make its clue affect another room, add a gated ending, test
both intended and impossible routes.
Checkpoint: draw a state diagram for rooms and endings.

## 9 — Regression tests
The smoke test checks arithmetic, concatenation, loops, branching, and a game
route. Test each operator, unknown variables, division by zero, and malformed
source. Verify invalid programs exit nonzero.
Checkpoint: assert expected output, not merely absence of a crash.

## 10 — Extend VNX
Choose break, functions, arrays, or block-local scope. Specify example syntax,
tokens, AST nodes, parser rules, runtime semantics, error cases, tests, and docs.
Implement incrementally and keep the game passing.

## 11 — Bootstrap and self-hosting
The interpreter is C bootstrap code. Next, implement a small interpreter in VNT
using supported features: arithmetic, variables, strings, then control flow.
Only after the language can express its own implementation should you attempt
self-hosting. This requires actually rebuilding the interpreter/compiler using
the language.

## Graduation project
Write a .vnx program with three variables, reassignment, a loop, nested
conditionals, input, concatenation, and two reachable endings. Add a deterministic
test and document the grammar and limitations.
