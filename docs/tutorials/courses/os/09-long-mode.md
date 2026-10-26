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

## 6. The control-register sequence has dependencies

Long mode is not enabled by one magic instruction. The bootstrap configures page-table state, enables the relevant CPU feature through the extended feature controls, enables paging/protected-mode state in CR0, and transfers into a 64-bit code segment. The GDT must contain a code descriptor appropriate for the far jump. A wrong ordering can cause a fault before the kernel has a usable console.

When reading the code, write the sequence in a table with columns for register or instruction, purpose, prerequisite, and next state. This is more useful than memorizing the hexadecimal constants because it lets you reason about why the sequence is ordered that way.

## 7. Why the far jump matters

Changing control bits does not automatically change the CPU's current instruction-decoding mode. The far jump loads a code-segment selector and a new instruction pointer, making the transition to the 64-bit code segment explicit. The target label must be assembled for the correct bitness and must be located where the processor can fetch it after paging is enabled.

If the jump target is wrong, the CPU may execute invalid bytes or fault before any VNT function can run. That is why early boot debugging needs a marker close to the transition rather than only a message in the final kernel entry.

## 8. What long mode does not provide

Long mode provides the 64-bit execution environment and architectural register set. It does not automatically create a heap, initialize devices, install a complete IDT, or establish userspace. The bootstrap's initial mapping is also not a memory allocator. Those are separate responsibilities built on top of the processor state.

The current kernel still runs in privileged kernel mode. A 64-bit address does not by itself make an access safe, and a pointer can still refer to unmapped or unintended memory.

## Lab: explain one dependency

Choose one control-register write or far jump in `os/boot.S`. State the CPU mode before the instruction, the feature it enables or selects, the prerequisite already established, and the first later instruction that relies on it. Then describe one plausible failure if the prerequisite were absent.

## 9. Validate the result, not just the source

After building, inspect the ELF entry point and disassembly. Confirm that the linked image contains the expected bootstrap and that the entry point points into the intended loadable region. Then boot the ISO in QEMU and look for the first kernel marker. These checks answer different questions: ELF inspection verifies the artifact's structure; QEMU verifies that the combined boot path executes far enough to observe the marker.

If the VM resets, do not immediately change the CPU feature bits. First establish whether GRUB loaded the image and whether the bootstrap reached the transition. Use a marker or debugger breakpoint at the earliest stage that can be observed safely.

## Lab: build a dependency table

For the long-mode transition, list the GDT, stack, page tables, CPU feature controls, paging bit, and far jump. For each, write what it depends on and which later operation relies on it. Then explain why moving the final jump earlier would violate the transition contract.

## 10. Keep the transition reproducible

Record the last known-good commit and the exact QEMU command before testing any change to long-mode setup. If the change causes an immediate reset, restore the baseline and reapply only the smallest part needed to reproduce the issue. Do not use a successful compiler exit as evidence that the CPU transition is correct.

A useful review should be able to explain why the GDT selector is valid, why the stack is mapped, why the page-table root is usable, and why the target address is reachable after paging becomes active. Those facts together explain the transition; a single "long mode enabled" comment does not.

## 11. What to include in a review

For a long-mode patch, state the intended processor state before the change, the state afterward, and the exact evidence used to validate the transition. Include the build result, ELF entry point, and QEMU observation. If the first visible marker is missing, say which stage you have ruled out and which remains uncertain.

This is a better review than "fixed boot" because another developer can reproduce the test and understand the architectural claim being made.
