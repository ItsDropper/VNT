# Lesson 09 — Enter x86-64 Long Mode

**Goal:** understand the main CPU-state transitions in the bootstrap.

## 1. Why the transition exists

The Multiboot2 entry used by this kernel begins in 32-bit protected mode. The generated kernel is x86-64 code, so the bootstrap must enable the CPU's long-mode environment before executing it. Writing `.code64` in an assembly file only controls instruction encoding; it does not configure the processor.

## 2. The high-level sequence

The current bootstrap follows this order:

1. Create initial page-table structures.
2. Enable PAE in `CR4`.
3. Load the top-level page-table address into `CR3`.
4. Set the Long Mode Enable (LME) bit in `EFER`.
5. Load a Global Descriptor Table (GDT).
6. Enable paging in `CR0`.
7. Perform a far jump into the 64-bit code label.
8. Set data segment registers and a valid aligned stack.
9. Call the generated entry point.

Order matters. Long mode requires compatible paging and CPU feature state. If one of these prerequisites is wrong, the machine may stop before any normal serial message appears.

## 3. The GDT

The Global Descriptor Table describes code and data segments. In 64-bit mode, segmentation works differently from classic 32-bit mode, but a valid code-segment descriptor is still needed for the transition.

In `boot.S`, the GDT includes a null descriptor, a 64-bit code descriptor, and a data descriptor. `lgdt` loads its pointer. The far jump selects the 64-bit code descriptor.

## 4. Stack alignment

Before calling `_start`, the code sets `RSP` to `boot_stack_top` and aligns it downward to a 16-byte boundary. Stack alignment is important for generated code and calling-convention expectations.

The stack is reserved in the bootstrap's `.bss` section. The kernel's later stack usage must remain within its allocated space; a stack overflow can corrupt adjacent data.

## 5. What this code does not do

This bootstrap is a minimal learning implementation. It does not implement a complete CPU-feature detection framework, a full exception-handling system, or a robust recovery path if the transition fails. The early page tables also map only the initial 1 GiB.

Do not copy this exact sequence into a different boot protocol or CPU target without consulting that platform's requirements.

## Checkpoint

Explain why these are separate things:
- assembler directive `.code64`;
- enabling long mode in CPU state;
- jumping to the 64-bit entry label.

## Exercise

Read `boot.S` from the first control-register update through `long_mode_entry`. Write down the purpose of each control-register operation and the far jump.