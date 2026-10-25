# 07 — Build a Parser

The parser consumes tokens and checks whether they match the grammar. It then builds AST nodes that represent the program.

A recursive-descent parser uses a function for each grammar rule. For example, `parse_statement` chooses between statement forms, while `parse_primary` recognizes literals, variable references, and parenthesized expressions.

## Parsing statements

A typical statement parser examines the next token:

- output keyword → parse an output statement;
- declaration keyword → parse a declaration;
- conditional keyword → parse a condition and branches;
- loop keyword → parse its condition and body;
- opening brace → parse a block;
- otherwise → report an unexpected token.

The parser must consume tokens when it succeeds. On failure, it should either stop with an error or recover in a deliberate way, such as skipping to the next statement boundary. Accidental non-advancement can create an infinite loop.

## Parsing expressions

If you add arithmetic, use a precedence hierarchy. One common structure is:

```text
expression -> equality
equality   -> comparison (("==" | "!=") comparison)*
comparison -> term (("<" | "<=" | ">" | ">=") term)*
term       -> factor (("+" | "-") factor)*
factor     -> unary (("*" | "/") unary)*
unary      -> ("!" | "-") unary | primary
primary    -> literal | identifier | "(" expression ")"
```

This is an example grammar. Do not document these operators as supported until the lexer, parser, and runtime all implement them.

## Error messages

An error should name the source file, line, and what was expected. “Expected closing parenthesis” is actionable; “parse error” usually is not.

## Exercise

Intentionally remove a closing quote, brace, or parenthesis from a test program. Improve the diagnostic so a beginner can find the problem.

## Checkpoint

Explain why a parser needs both a grammar and a consistent token stream.
