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

## 2. Add exception and interrupt handling

A kernel needs a defined response to faults such as invalid instructions, page faults, and divide errors. The next architecture work includes an Interrupt Descriptor Table (IDT), well-defined assembly entry stubs, register-save conventions, and safe reporting of exception state.

Interrupt entry code is a good reason for a small assembly component. Ordinary exception policy and reporting can remain in VNT when the compiler and ABI support it.

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