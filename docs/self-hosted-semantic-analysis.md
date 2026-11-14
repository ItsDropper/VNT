# VNT self-hosting: semantic analysis

The VNT-native frontend now has three independently callable stages:

1. `compiler/lexer.vnt` exports `lex(source)`.
2. `compiler/parser.vnt` exports `parse(source)` and `parse_program(source)`.
3. `compiler/semantic.vnt` exports `analyze(program)`.

The semantic pass consumes the parser's VNT object AST directly. It does not construct
or call the hosted C `AstNode`, symbol table, or type checker.

## Implemented semantic checks

- Whole-program collection of function and struct declarations, enabling forward calls.
- Duplicate function/struct declarations and duplicate struct fields.
- Lexical scopes for top-level code, function bodies, branches, loops, and unsafe blocks.
- Explicit and inferred variable types, same-scope redeclaration checks, and assignment compatibility.
- Integer, float, boolean, string, null/pointer, array, object, reference, and void categories.
- Unary and binary operator validation, including numeric promotion, boolean logic,
  comparisons, equality, string concatenation, indexing, and member access.
- Function arity, parameter types, return types, and missing typed return paths.
- Loop-only `break`/`continue`, iterable validation for `for-in`, and unsafe-only assembly.
- Builtin result typing for the runtime APIs currently represented by the C checker.

## Deliberate boundaries

This is a first semantic parity milestone, not a claim of full C-checker equivalence.
The current parser represents struct field names but not field types, so member access
can only reject void receivers and otherwise remains dynamically typed. The current
for-in AST has no element-type metadata, so its binding is unknown. The VNT pass reports
the first semantic diagnostic, matching the hosted checker's current fail-fast behavior.
Unqualified sibling-export resolution and branch-state merging are implemented. The complete per-builtin result-type table and richer struct-field typing remain follow-up work.

Run `tests/self_hosted_semantic.vnt` through the self-hosted frontend test workflow.
