# VNT Compiler Architecture and AST Retirement

## Goal

The AST is a frontend implementation detail. Once parsing and semantic analysis finish, the compiler pipeline must operate on a self-contained intermediate representation (IR). Optimization and native code generation must not inspect AST nodes or rely on pointers into the AST.

This is a migration contract, not a claim that the current implementation has completed the migration.

## Required pipeline

```text
source
  -> lexer/parser
  -> temporary AST
  -> semantic analysis (diagnostics + resolved symbols/types)
  -> self-contained HIR
  -> HIR validation
  -> HIR optimization passes
  -> native backend
  -> assembler/linker
```

The AST can be destroyed immediately after HIR lowering and semantic information has been transferred. No HIR node may retain an AST pointer.

## HIR invariants

1. **Ownership:** names, literal text, type metadata, source spans, and symbol references are owned by the HIR/module arena or referenced by stable IDs.
2. **No AST dependencies:** the HIR public header and optimizer/backend implementations must not require `AstNode`, `AST_*`, or source-AST pointers.
3. **Validation:** every node has a valid opcode, operation, type, source span, and child-edge shape. The graph is connected, acyclic, and has exactly one root.
4. **Semantics first:** name resolution and type checking finish before optimization. The backend does not infer language semantics from syntax.
5. **Pass contracts:** each optimization pass takes validated HIR and returns validated HIR. Passes must preserve observable behavior, including evaluation order, short-circuiting, overflow, division-by-zero, and reference semantics.
6. **Diagnostics:** errors retain source spans after the AST is destroyed.
7. **Backend contract:** code generation consumes HIR only. It must not reconstruct an AST as an intermediate workaround.

## Migration stages

### Stage 1 — Make HIR self-contained
- Copy all required node data, source spans, type information, and resolved symbol IDs into HIR.
- Remove `VntIrNode.source` and `VntIrProgram.program`.
- Make HIR validation check IR invariants rather than comparing against AST nodes.
- Keep the AST alive only until lowering and semantic analysis complete.

### Stage 2 — Move optimization to HIR
- Implement constant folding over IR nodes, with safe graph rewriting and node compaction/arena management.
- Add explicit pass APIs and validate after each pass.
- Add tests for signed integer edge cases, floating-point behavior, short-circuit logic, and side effects.
- Never mutate the AST during optimization.

### Stage 3 — Migrate native code generation
- Replace AST traversal in the x86-64 backend with HIR traversal.
- Derive variable/symbol layouts and control flow from HIR metadata.
- Keep runtime ABI and executable output stable while migrating.
- Remove the transitional AST backend path only after all language constructs are covered.

### Stage 4 — Prove the migration
- Add tests that lower a program, free its AST, and then optimize, validate, dump, and compile the HIR.
- Run the complete correctness suite and benchmark suite.
- Compare generated program outputs against the pre-migration compiler for the supported corpus.
- Do not declare AST retirement complete until the AST-free compile test and all regression tests pass.

## Current known blocker

The current implementation does not satisfy this contract yet: `VntIrProgram` retains the source AST, `vnt_ir_optimize` folds AST nodes and rebuilds HIR, and `src/compiler/x86_backend.c` traverses the AST through `ir->program`. These dependencies must be removed in the stages above; adding more optimization rules before that migration would deepen the coupling.
