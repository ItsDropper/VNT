# VNT systems-programming roadmap

This document tracks the compiler/runtime work needed to move VNT from a hosted native language toward systems programming and freestanding kernel development. A feature is not complete merely because its syntax or documentation exists: each stage requires compiler/runtime tests and a CI build.

## Required milestones

### 1. Pointer-width integer types: `usize` and `isize`
- Add distinct semantic types, not aliases for the existing signed 32-bit `int`.
- Define their width from the selected target ABI (64-bit for the initial x86-64 target).
- Preserve signedness and width through type checking, HIR, code generation, and function ABI boundaries.
- Reject invalid implicit conversions; add explicit conversion rules and boundary tests.

### 2. General 64-bit integer semantics
- Add signed and unsigned 64-bit integer representations end-to-end, including literal parsing, constant folding, arithmetic, comparisons, overflow behavior, function arguments/returns, and printing.
- Test values above 32-bit range, negative signed values, carry/overflow edges, and division corner cases.
- Do not route 64-bit values through `double` or truncate them to C `int`.

### 3. 64-bit memory access
- Add `read_u64`, `write_u64`, and signed counterparts only when their value types are represented without truncation.
- Validate address arithmetic overflow and preserve the existing unsafe-context requirement.
- Test exact byte layout, alignment-independent access, offsets, and values above `UINT32_MAX`.

### 4. Operand-aware inline assembly
- Extend parsing/AST/IR beyond the current single raw assembly template.
- Model input/output operands, constraints, clobbers, and volatile semantics explicitly.
- Validate operand counts and constraints; ensure generated calls preserve the target ABI and register/stack rules.
- Add positive assembly tests and negative diagnostics tests. Raw template substitution must not silently accept unsupported constraints.

### 5. Freestanding compiler/linker pipeline
- Add a separate freestanding target mode instead of invoking the hosted runtime/link command unconditionally.
- Make target triple, object format, entry symbol, linker script, compiler flags, and runtime selection explicit.
- Ensure no accidental libc, hosted startup object, dynamic loader, or hosted runtime dependencies enter the freestanding image.
- Keep hosted compilation behavior and its CI coverage unchanged.

### 6. Demonstrated bootable VNT kernel
- Create a minimal freestanding kernel entry point and linker script for a documented boot path (initially x86-64 QEMU).
- Produce a bootable image through CI, boot it in an emulator, and assert a serial marker from the kernel.
- Include reproducible build/run instructions and retain the boot log as CI evidence.
- Expand to interrupts, memory management, and hardware access only after the minimal image boots reliably.

## Acceptance rule

Each milestone must land with executable regression tests and CI evidence. Documentation, a parser-only implementation, or a hosted executable is not evidence that a freestanding kernel boots. Do not mark a milestone complete until its acceptance tests pass on the PR branch.
