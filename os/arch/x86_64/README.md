# x86-64 architecture boundary

Copyright (c) 2026 ItsDropper  
Licensed under the MIT License. See the repository root `LICENSE`.

This directory owns the instructions and data structures that are specific to
the current x86-64 boot/interrupt path.

- `boot.S` contains the Multiboot2 entry, initial page-table setup, long-mode
  transition, bootstrap stack, and current shared scalar storage.
- `interrupts.S` contains IDT gate encoding, exception/IRQ entry stubs, the
  register-save/restore boundary, PIC remapping, PIT programming, and `iretq`.

The VNT policy code stays in `os/kernel.vnt` and `os/interrupts.vnt`. In
particular, `interrupt_dispatch` decides what an interrupt means; the assembly
stub only establishes the CPU frame and returns through the architecture-defined
interrupt-return instruction.

## Constraints not solved by this move

This is a real source/build separation, not a finished architecture abstraction.
The boot file still defines `vntos_state`, which VNT functions access with
inline assembly. Several VNT routines depend on compiler-generated stack
offsets. Imported VNT modules are flattened into one compilation unit, and the
current compiler does not yet provide a robust typed boundary for port I/O,
CPU state, interrupt frames, or context switching.

Do not implement a scheduler, general allocator, or interrupt-driven keyboard
on top of these constraints without first establishing and testing the relevant
ABI and ownership contracts. Keep IRQ1 masked while input remains polled.
