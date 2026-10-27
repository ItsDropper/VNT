# x86-64 architecture boundary

Copyright (c) 2026 ItsDropper  
Licensed under the MIT License. See the repository root `LICENSE`.

This directory owns the x86-64 bootstrap, CPU entry/exit, and architecture
operations that cannot yet be expressed through a stable VNT ABI.

- `boot.S` contains Multiboot2 entry, initial page tables, long-mode transition,
  the bootstrap stack, and shared scalar storage. It identity-maps the first
  GiB for early kernel memory and separately maps the conventional PC IOAPIC
  (`0xFEC00000`) and Local APIC (`0xFEE00000`) 2-MiB MMIO windows.
- `interrupts.S` contains IDT gate encoding, exception/IRQ/IPI entry stubs,
  register save/restore, legacy PIC/PIT setup, and `iretq`.
- `memory.S` provides bounded boot-memory/physical-bitmap primitives, while
  `virtual_memory.S` provides early page-table operations.
- `ap_trampoline.S` performs the unavoidable real-mode/early-mode transition
  needed to start an application processor. The AP then enters VNT code.

ACPI discovery and MADT parsing live in `os/firmware/acpi.vnt`; Local APIC
and I/O APIC policy live in `os/arch/x86_64/apic.vnt`; CPU topology and IPI
bookkeeping live in `os/kernel/smp.vnt`.

## Current limitations

This is early SMP bring-up, not a finished SMP kernel. APs are started serially
through a copied real-mode trampoline, each with a private stack. They load the
shared IDT, initialize their local APIC, publish online state, and enter an
interruptible idle loop. Vector 240 is used for a bounded IPI response handler.
The BSP's QEMU self-test checks delivery to each online AP.

There is still no per-CPU GDT/TSS or IST stack, scheduler/context switch, or
SMP-safe physical allocator. The IPI response counter and allocator bitmap
operations are not general synchronization primitives. Do not run ordinary
allocator or scheduler work concurrently on APs until atomics, locking,
ownership, and interrupt-context rules are implemented and tested.

Only conventional xAPIC MMIO addresses are mapped. x2APIC mode, nonstandard
Local APIC/I/O APIC address overrides, multiple I/O APIC controllers, and
interrupt-source override routing are not activated. Legacy PIC/PIT timer
handling remains in place; keyboard IRQ1 remains masked while input is polled.
