# VNT systems-programming roadmap

This roadmap moves VNT from a hosted native language toward freestanding kernel development. Milestones are implemented and tested in sequence; parser-only or documentation-only changes do not count as completed compiler functionality.

## 1. Freestanding compiler and linker path — initial subset implemented

The branch now provides `--emit-freestanding-asm` and `--build-freestanding` for an ELF64 x86-64 target. The supported subset includes signed 32-bit `int`/booleans, local variables, arithmetic, comparisons, `if`/`while`, loop control, returns, and inline assembly. The entry stub sets up a private stack, invokes zero-argument `kernel_main()`, and halts if it returns. Signed arithmetic overflow traps with `ud2`.

- Linux and Windows CI passed on [run #262](https://github.com/ItsDropper/VNT/actions/runs/40182533521).
- The Linux test links with `ld -nostdlib -static`, verifies ELF64 and load segments, checks for undefined symbols, and verifies hosted `print` is rejected.
- Unsupported hosted values/APIs and function calls are rejected instead of introducing runtime dependencies.
- This output is **not bootable yet**: the entry assumes the processor is already in 64-bit mode. The boot protocol and QEMU test are milestone 6.

## 2. Pointer-width and 64-bit integer semantics

- Add genuine `usize` and `isize` types, distinct from the existing signed 32-bit `int`.
- Add explicit signed/unsigned 64-bit value types and define widths from the selected target ABI (64-bit for x86-64).
- Preserve width and signedness through literal parsing, type checking, HIR, constant folding, runtime representation, arithmetic, comparisons, printing, and function ABI boundaries.
- Define conversion rules and reject invalid implicit conversions.
- Test values above 32-bit range, negative signed values, carry/overflow edges, division corner cases, and unsigned boundaries. Do not route 64-bit values through `double` or truncate them to C `int`.

## 3. 64-bit memory access

- Add `read_u64`, `write_u64`, and signed counterparts only when their value types preserve all 64 bits.
- Validate address arithmetic overflow and preserve the existing unsafe-context requirement.
- Test exact byte layout, alignment-independent access, offsets, and values above `UINT32_MAX`.

## 4. Operand-aware inline assembly

- Extend parsing, AST, and HIR beyond the current single raw assembly template.
- Model input/output operands, constraints, clobbers, and volatile semantics explicitly.
- Validate operand counts and constraints; ensure generated code preserves target ABI and register/stack rules.
- Add positive assembly tests and negative diagnostic tests. Unsupported constraints must not be silently accepted.

## 5. Kernel boot contract and platform startup

- Select and document a real boot path for x86-64 (GRUB Multiboot2 with a 32-bit transition stub, or another explicitly supported loader contract).
- Add the necessary boot header, CPU mode transition, stack setup, linker layout, and entry ABI.
- Keep the freestanding ELF free of libc, hosted startup objects, dynamic loader, and VNT hosted runtime.
- Add static validation of the resulting image before emulator testing.

## 6. Demonstrated bootable VNT kernel

- Build a minimal VNT kernel image in CI and boot it in QEMU.
- Assert a known serial marker from kernel code and retain the serial log as CI evidence.
- Document reproducible build and run commands.
- Expand to interrupts, memory management, and hardware access only after the minimal image boots reliably.

## Acceptance rule

Each milestone requires executable regression tests and CI evidence. The freestanding subset in milestone 1 is implemented, but it is not a bootable OS. Keep the PR open until remaining milestones pass; do not equate assembly generation or a linkable ELF with a kernel that has successfully booted.
