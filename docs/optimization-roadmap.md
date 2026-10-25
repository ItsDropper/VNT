# VNT Compiler Optimization Roadmap

This document tracks implementation work against observable compiler behavior. It is not a claim that planned features already exist.

## Baseline (existing implementation)

- AST-to-HIR lowering with HIR-owned payloads.
- HIR validation and bounded fixed-point optimization.
- Integer/boolean/floating-point constant folding where semantics are defined.
- Conservative constant propagation, including invalidation at control-flow merges, loops, calls, and indirect writes.
- Constant-branch simplification and pruning after unconditional terminators.
- Direct HIR traversal by the x86-64 backend.
- Runtime benchmark harness in `benchmarks/run.ps1`, including output checks, warmups, repeated timing, medians, and CSV output.

## Implementation sequence

### Phase 0 — correctness and measurement (current gate)

- Keep every optimization covered by both HIR-level invariants and compile-and-run tests.
- Preserve overflow behavior: never fold signed overflow into a wrapped result when the language traps or leaves the operation unfolderable.
- Treat calls, raw pointers, references, indirect assignments, and memory effects conservatively.
- Capture benchmark results before and after compiler changes on the same machine and toolchain.
- Report runtime, executable size, compile time, and correctness separately. Do not claim a speedup from a single run.

### Phase 1 — CFG and SSA foundation

- Introduce an explicit basic-block representation with predecessor/successor edges.
- Define stable value IDs and explicit terminators before implementing SSA.
- Add dominance information and verified phi-node placement.
- Add verifier tests for malformed edges, unreachable blocks, dominance violations, and invalid phi inputs.
- Keep the current HIR pipeline as the supported path until the new representation passes differential tests.

### Phase 2 — safe scalar optimization

- Dead-code elimination based on explicit side-effect classification.
- Copy propagation and sparse conditional constant propagation.
- Common-subexpression elimination only when operands and memory effects make reuse sound.
- Algebraic simplification constrained by VNT's exact integer, floating-point, and evaluation semantics.
- Differential tests comparing optimized and unoptimized observable output.

### Phase 3 — backend quality

- Liveness analysis and register allocation.
- Instruction selection/combining and reduced redundant loads/stores.
- Calling-convention tests for both Windows x64 and System V AMD64 where supported.
- Assembly inspection tests for specific properties, backed by runtime tests rather than used as the sole correctness gate.

### Phase 4 — interprocedural optimization

- Small-function inlining with code-size limits.
- Interprocedural constant propagation and dead internal-function elimination.
- Link-time optimization only after module boundaries and symbol visibility are explicit.

### Phase 5 — loops and profiles

- Loop-invariant code motion and induction-variable simplification after CFG/SSA verification.
- Profile-guided optimization only after profile format/versioning, deterministic fallbacks, and representative workloads exist.

## Acceptance criteria for every pass

1. A focused regression test demonstrates the intended transformation.
2. Negative tests prove unsafe cases are not transformed.
3. The optimized HIR passes structural validation.
4. End-to-end compiled programs preserve expected output and exit status.
5. The benchmark suite remains correct; performance claims include repeated measurements and toolchain/environment details.
6. If a pass increases runtime or code size materially, its heuristic or default enablement is reconsidered.

## Non-goals for the first optimization milestone

- Do not replace the compiler pipeline wholesale.
- Do not assume pointers never alias.
- Do not treat `volatile` as a substitute for atomics or memory ordering.
- Do not enable aggressive loop transforms or PGO without correctness and measurement infrastructure.
- Do not claim SSA, register allocation, LTO, or PGO support until they are implemented and tested.
