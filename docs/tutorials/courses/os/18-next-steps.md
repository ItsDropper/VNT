# Lesson 18 — What Comes Next?

**Goal:** turn the course's small working kernel into a realistic systems-learning roadmap.

Completing this course means you understand the current boot path and can make a small tested shell change. It does not mean VNTOS is a complete operating system. The next step is to add one subsystem at a time with a design, explicit limitations, and tests.

## 1. Improve the VNT freestanding backend

The kernel currently relies on low-level inline assembly, including compiler-specific stack offsets. Safer compiler support should provide explicit outputs/constraints or dedicated intrinsics for hardware operations and linker symbols.

Useful goals:
- define and document the freestanding ABI;
- add tests for generated code and calling conventions;
- reject unsupported features with clear errors;
- remove reliance on incidental stack-frame layout;
- add dedicated port-I/O and CPU-control intrinsics with architecture checks.

These are compiler and ABI tasks, not just changes to the shell source.

## 2. Extend the interrupt subsystem carefully

The current kernel now has a 256-entry IDT, exception stubs, PIC remapping, a PIT timer IRQ, VNT-side dispatch, and fatal exception reporting. The `irq` shell command lets you observe the timer counter. That is a first interrupt path, not a finished subsystem.

The keyboard and mouse still use polling, and only IRQ0 is unmasked. Before enabling keyboard IRQ1, design an interrupt-safe queue and make the IRQ handler the sole owner of reads from the PS/2 data port. Do not let the current polling loop and an IRQ handler race to consume the same byte.

The default IDT stub currently reports a fallback marker rather than preserving the original vector for every unused gate. Improving unexpected-vector diagnostics is a useful contained task. A later architecture pass should also define compiler-supported interrupt-safe entry primitives and a documented register/stack ABI.

## 3. Build a real memory subsystem

Before allocating arbitrary memory, the kernel needs to parse bootloader memory information, reserve occupied regions, manage free physical pages, and build deliberate virtual mappings. The current 1 GiB identity map is only an initial bootstrap mapping.

A physical-page allocator, virtual-memory manager, heap, and page-fault handler are related but distinct pieces. Implement and test them incrementally.

## 4. Add a timer and scheduling model

A scheduler needs a way to represent tasks, save and restore execution state, choose which task runs next, and respond to timer events. This should not be attempted before the kernel has reliable interrupt entry, memory ownership, and a defined context-switch ABI.

## 5. Add user mode and system calls

User-mode execution requires isolation, controlled transitions between privilege levels, memory protection, a syscall interface, and a way to load executable programs. A kernel shell is not equivalent to user mode.

## 6. Add storage and drivers

A filesystem depends on a reliable storage interface, block I/O, error handling, and memory buffers. Device discovery and drivers need hardware-specific documentation and tests. Do not claim a filesystem exists until files can actually be read and written through a tested implementation.

## 7. Make every subsystem testable

For each subsystem, document:
- the interface and invariants;
- what is implemented and what is not;
- emulator coverage and any hardware-only checks;
- error paths and resource ownership;
- how a regression is detected.

A useful roadmap is a list of verifiable capabilities, not a list of impressive names.

## Final assessment

You are ready to continue if you can do all of the following without copying a recipe blindly:

- explain the GRUB → Multiboot2 → assembly bootstrap → generated entry → `kernel_main` path;
- explain why the linker script and initial page tables are needed;
- build and boot VNTOS in QEMU;
- explain the current serial driver's busy-wait behavior and limitations;
- add a shell command in VNT and test it;
- distinguish a successful build from a verified runtime result;
- identify at least three important capabilities VNTOS does not yet implement.

## Final note

The right way to grow a kernel is not to add every subsystem at once. Understand the current contract, add one carefully scoped capability, test it at the appropriate level, and record the evidence.

## 8. Order future work by dependency

The next subsystem should be chosen by the prerequisites it needs, not by how impressive the feature sounds. A physical-page allocator needs a trustworthy memory map and reserved-range handling. A heap needs a source of pages and allocation invariants. User mode needs page permissions and controlled entry/exit. A scheduler needs a defined saved-context layout and a timer path that can safely select another task.

Keyboard IRQ handling is not simply unmasking IRQ1. The PS/2 data port currently has a polling consumer. An IRQ design must establish one owner for device reads and provide a bounded queue so the interrupt handler can return quickly. Without that queue and ownership rule, polling and interrupts can race and lose bytes.

## 9. A practical staged roadmap

A defensible progression is:

1. Improve observability and test assertions around the existing boot path.
2. Document and test the VNT/assembly ABI.
3. Replace compiler-specific stack-offset tricks with supported compiler features.
4. Add named, bounded kernel state interfaces.
5. Define a physical memory map and page-frame allocator.
6. Add a small heap only after physical allocation is trustworthy.
7. Add interrupt-safe event queues before enabling more device IRQs.
8. Add user mode only after memory permissions and syscall transitions exist.
9. Add scheduling only after context save/restore and timer semantics are explicit.
10. Add persistent storage after a driver and filesystem boundary are defined.

This is a dependency order, not a promise that all features already exist.

## 10. Choose a graduation project with measurable success

Prefer a project that can be built and tested within one focused change. A stronger exception report is measurable if it reports the vector and error code and preserves the fatal halt. A timer statistics command is measurable if its counter and units are documented. A compiler ABI test is measurable if it catches an unexpected symbol or stack-layout change.

Avoid goals such as "make the OS faster" unless you first choose a workload, define a measurement, and capture a baseline. Kernel progress is meaningful when the behavior and its evidence are reproducible.
