# VNT Compiler Architecture

## Current pipeline

VNT uses an AST in the frontend, then lowers the program into a self-contained high-level intermediate representation (HIR). The AST is not a runtime requirement of HIR optimization or native code generation.

```text
VNT source
    |
    v
Lexer and parser
    |
    v
AST
    |
    v
Semantic/type analysis
    |
    v
HIR lowering (owned payloads, index-addressed nodes)
    |
    v
HIR validation and optimization
    |
    v
Control-flow graph + definite-assignment analysis
    |
    v
HIR-native x86-64 backend
    |
    v
GCC + native runtime
    |
    v
Native executable
```

## HIR ownership and structure

The HIR is represented by `VntIrProgram` and `VntIrNode` in `include/vnt/ir.h`.

- Nodes live in a growable array and refer to children and siblings by stable numeric indexes, not pointers into the AST.
- Text payloads, names, and explicit type annotation strings are copied into HIR-owned allocations during lowering.
- Node child edges have explicit roles such as condition, body, statement, operand, target, and value.
- `vnt_ir_validate` validates HIR structure and node shape using HIR-owned data.
- `vnt_ir_optimize` operates on the HIR graph rather than rewriting AST nodes.
- `src/compiler/x86_backend.c` traverses HIR opcodes and edge roles directly to emit x86-64 assembly.

The frontend still parses source into an AST, and semantic/type checking currently takes that AST as input. That is normal frontend use; it does **not** mean the HIR backend requires the AST to remain alive.

## AST-free regression coverage

`tests/hir_ast_free_native.c` explicitly verifies the boundary:

1. Construct a small AST and lower it into HIR.
2. Destroy the AST.
3. Validate and optimize the HIR after AST destruction.
4. Validate and dump the HIR.
5. Emit native x86-64 assembly from that HIR and check the expected assembly markers.

This test is intended to catch accidental reintroduction of AST dependencies in HIR optimization or code generation. It proves the tested HIR path can operate after AST destruction; it does not, by itself, prove every language construct or generated executable is correct. The full language and runtime regression suites remain necessary.

## Remaining work

The existence of an AST-free HIR backend does not mean the compiler is finished. Useful future work includes:

- Broader regression coverage for every HIR opcode and interactions between features.
- Stronger semantic/type information in HIR, including resolved symbol metadata where needed.
- More robust diagnostics that preserve source locations across all lowering and optimization cases.
- Additional HIR optimization passes with explicit correctness tests.
- Broader end-to-end tests that compile and execute representative programs, not just inspect emitted assembly.
- Keeping the control-flow graph and definite-assignment analysis consistent with language semantics.

Do not describe the backend as rebuilding or traversing an AST: its current implementation consumes HIR directly. Distinguish this from the frontend, which still uses an AST for parsing and type checking.
