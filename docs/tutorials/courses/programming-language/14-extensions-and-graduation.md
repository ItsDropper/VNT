# 14 — Extensions and Graduation Project

After the core interpreter works, extend it one feature at a time. For each feature, update the language specification, lexer, parser, AST, runtime, tests, and reference as needed.

## Good next features

- **Functions**: define call syntax, parameters, argument evaluation, return values, and call-stack behavior.
- **Arrays**: define indexing, bounds errors, mutation, and value semantics.
- **Block scope**: decide how nested environments search and update variables.
- **Better diagnostics**: store source columns and point to the exact offending token.
- **Bytecode VM**: compile the AST into a compact instruction sequence and execute it in a virtual machine.
- **Static types**: define types and type-checking rules before runtime.
- **Native compilation**: only after the language semantics are stable, design a backend and calling convention.

Do not add features merely because another language has them. Add them when they solve a problem and you can implement and test them properly.

## Graduation project

Build a language program that:

1. prints a title and instructions;
2. reads player input;
3. stores at least three pieces of state;
4. makes decisions based on earlier actions;
5. contains a repeatable interaction or a clear multi-step route;
6. offers at least two distinct endings;
7. rejects invalid choices gracefully.

Then submit a short design note describing the grammar, AST node types, runtime values, limitations, and tests.

## Final review

- [ ] The interpreter implementation is written in VNT.
- [ ] The language source is separate from interpreter source.
- [ ] The lexer always advances and emits EOF.
- [ ] The parser reports malformed syntax.
- [ ] The AST preserves operator precedence.
- [ ] Runtime errors are explicit.
- [ ] Tests cover both successful and failing programs.
- [ ] The adventure runs without interpreter-specific hardcoding.
- [ ] Documentation matches the actual implementation.

Finishing a small language correctly is more valuable than claiming a large language with unimplemented features.
