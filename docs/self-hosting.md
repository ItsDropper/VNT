# VNT self-hosting

VNT self-hosting development has started on `main`. The first component is a lexer implemented in VNT itself at [`compiler/lexer.vnt`](../compiler/lexer.vnt).

## Bootstrap strategy

The existing C compiler remains the trusted bootstrap compiler while the VNT implementation is developed incrementally. The C compiler is not being removed or bypassed.

The intended bootstrap sequence is:

1. The existing C compiler builds and runs VNT source.
2. VNT source implements compiler components, beginning with the lexer.
3. The VNT-native components are assembled into a complete compiler frontend and then a complete compiler.
4. The C compiler builds the VNT compiler executable.
5. That VNT compiler compiles its own source tree; a subsequent generation repeats the build.
6. CI checks language behavior and bootstrap reproducibility before the VNT compiler can become the default.

A successful self-compilation alone is not sufficient evidence of correctness. The bootstrap must also preserve behavior, pass regression tests, and produce stable compiler outputs.

## Milestone 1: lexer

`compiler/lexer.vnt` exports:

```vnt
import "compiler/lexer.vnt" as lexer

result = lexer.lex(source)
```

The result object contains:

- `tokens`: ordered token objects, each with `kind`, `lexeme`, `line`, and `column`.
- `errors`: diagnostics for unknown characters and unterminated strings.
- `ok`: `true` only when no lexical diagnostics were produced.

Token kind names correspond to the `TokenType` enum in `include/vnt/lexer.h`. String token lexemes preserve the source spelling of escape sequences, matching the current C lexer; escape decoding remains the parser's responsibility. Comments and whitespace are skipped, and one EOF token is always appended.

The initial regression program is [`tests/self_hosted_lexer.vnt`](../tests/self_hosted_lexer.vnt). It exercises identifiers, decimal and hexadecimal numbers, strings, punctuation, all current operator token families, comments, source locations, unknown characters, and unterminated strings. It is included in `tests/run_language_checks.ps1`.

This is an initial behavioral-compatibility slice, not yet a claim that the VNT lexer has complete parity for every locale or every possible source byte. The production lexer uses C character-classification routines; the VNT implementation currently defines ASCII identifier and numeric classes explicitly.

## Milestone 2: expression parser

`compiler/parser.vnt` imports the native lexer and exports `parse(source)`. It returns `ok`, `error`, and an object-graph expression tree. The initial grammar covers literals, variables, calls (including module-qualified calls lowered to the existing `module__function` naming convention), array literals, indexing, member access, unary operators, and the full current binary-operator precedence ladder. String escape decoding follows the current C parser for the supported escape set.

The parser regression program is [`tests/self_hosted_parser.vnt`](../tests/self_hosted_parser.vnt). It checks precedence and associativity, nested expressions, calls, arrays, postfix expressions, escape decoding, and malformed input. It is part of `tests/run_language_checks.ps1`.

This is deliberately an expression-parser milestone, not a complete statement parser or a drop-in replacement for `parser_parse`. The syntax tree is an owned VNT object graph and is not yet converted into the C `AstNode` layout or HIR.

## Milestone 3: statements and declarations

`compiler/parser.vnt` now exports `parse_program(source)`, returning `ok`, `error`, and an ordered `statements` array. The statement grammar includes:

- Typed `let` declarations and inferred assignment/update statements
- Variable, member, index, and dereference assignments, including compound assignment
- Function-call statements and `print(...)`
- Value and bare `return`, `break`, and `continue`
- `if` / `else if` / `else`, `while`, and `for name in expression`
- Typed function parameters, optional return types, and unsafe function declarations
- Struct field lists, unsafe blocks, and `asm` / `asm volatile` statements

Blocks contain ordered statement arrays. The native tree is intentionally a VNT-owned syntax model, not a C `AstNode` clone; notably, `for` is represented as a `FOR` node rather than being desugared to the C parser's generated while-loop AST.

The regression program is [`tests/self_hosted_statement_parser.vnt`](../tests/self_hosted_statement_parser.vnt), integrated into `tests/run_language_checks.ps1`. It covers a representative mixed program, nested control flow, typed declarations, function/struct metadata, unsafe/assembly forms, and malformed blocks/declarations.

This is a grammar milestone, not yet semantic parity. Name resolution, loop-context validation, type checking, precise diagnostics, and HIR lowering remain separate compiler phases.

## Next implementation order

1. Execute and stabilize the lexer, expression-parser, and statement-parser suites on the supported Windows build environment.
2. Compare the native parser against the C parser on a shared valid/invalid source corpus, including numeric literal boundaries and assignment edge cases.
3. Implement native semantic analysis and type checking using the current language rules.
4. Lower the VNT-native syntax representation into the existing HIR model.
5. Validate, optimize, and emit native code using the established HIR/backend contracts.

The existing HIR and x86-64 backend are important assets: the first self-hosted compiler does not need to redesign the backend. It needs to reproduce the frontend and lowering contracts accurately, then reuse or progressively port the backend.

## Development rule

Keep the C compiler operational throughout the transition. Every VNT-native component must have executable regression coverage, and each stage should be compared against the C implementation on the same input corpus. Do not switch the default compiler until the bootstrap and parity gates are demonstrated in CI.
