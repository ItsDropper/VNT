# x86-64 architecture boundary

Copyright (c) 2026 ItsDropper  
Licensed under the MIT License. See the repository root `LICENSE`.

This directory owns the x86-64 bootstrap, CPU entry/exit, and architecture
operations that cannot yet be expressed through a stable VNT ABI.

- `boot.S` contains Multiboot2 entry, initial page tables, long-mode transition,
  the bootstrap stack, and shared scalar storage. It identity-maps the first
  GiB for early kernel memory and separately maps the conventional PC IOAPIC
  (`0xFEC00000`) and Local APIC (`0xFEE00000`) 2-MiB MMIO windows.
- `interrupts.S` contains IDT gate encoding, exception/IRQ entry stubs,
  register save/restore, legacy PIC/PIT setup, and `iretq`.
- `apic.vnt` owns Local APIC enablement and I/O APIC register policy in VNT.
  The I/O APIC is only initialized when the MADT describes one controller at
  the mapped conventional address; redirection entries are masked, not routed.

ACPI discovery and MADT parsing live in `os/firmware/acpi.vnt`; CPU topology
bookkeeping lives in `os/kernel/smp.vnt`. The parser validates RSDP and table
checksums, table bounds, and MADT record lengths before using entries.

## Current limitations

This is early SMP bring-up, not a finished SMP kernel. MADT CPUs are started serially through a copied real-mode trampoline; each AP receives a private stack, enters VNT, publishes online state, and halts with interrupts disabled. There is no per-CPU IDT/TSS, APIC-vector IPI handler, scheduler/context switch, or SMP-safe allocator. The current physical allocator is still single-core and limited to the first GiB.

Only conventional xAPIC MMIO addresses are mapped. x2APIC mode, nonstandard
Local APIC/I/O APIC address overrides, multiple I/O APIC controllers, and
interrupt-source override routing are parsed or detected but not activated.
Legacy PIC/PIT timer handling remains in place. Keep keyboard IRQ1 masked while
input is polled, and do not unmask I/O APIC routes before installing handlers.
