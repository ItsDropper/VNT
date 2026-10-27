# Lesson 27 — ACPI, APIC, and the Road to SMP

**Goal:** understand how VNTOS discovers processors and interrupt controllers
without confusing firmware topology with CPUs that are actually executing.

This milestone adds bounded ACPI RSDP/root-table/MADT parsing, conventional PC
APIC MMIO mappings, Local APIC enablement, masked I/O APIC initialization, and
a CPU-topology diagnostic command. It does **not** start secondary processors.

## 1. ACPI discovery

Multiboot2 can provide an ACPI RSDP tag. The RSDP points to an RSDT (32-bit
table addresses) or, for newer ACPI revisions, an XSDT (64-bit addresses).
The root table contains pointers to other ACPI tables. VNTOS searches for the
MADT, whose signature is `APIC`.

The parser validates the RSDP checksum, the extended checksum when present,
table lengths, table checksums, address bounds, and every MADT record length
before reading its fields. In this kernel milestone, firmware tables must be
inside the bootstrap's first-GiB identity map. A table outside that range is
rejected rather than dereferenced through an unmapped address.

## 2. MADT records

The MADT can describe Local APIC processor entries, x2APIC processor entries,
I/O APIC controllers, interrupt-source overrides, and a Local APIC address
override. VNTOS records up to sixteen APIC IDs and interrupt-source override
triples for diagnostics, while retaining the full count of enabled/online-
capable processor entries.

The existence of an APIC ID in the MADT is not proof that its CPU is online.
The `smp` command reports discovered and online CPU counts separately.

## 3. Local APIC and I/O APIC

The bootstrap maps the conventional PC MMIO windows at `0xFEC00000` and
`0xFEE00000` through a dedicated page directory. The Local APIC is enabled
through IA32_APIC_BASE, its task-priority register is cleared, and its
spurious vector is set to 255. The dispatcher treats vector 255 as spurious;
it does not send an EOI for that vector.

When exactly one I/O APIC is present at the supported address, VNTOS reads its
version and masks its redirection entries. It does not route external IRQs yet.
PIC/PIT handling remains active, and keyboard IRQ1 remains masked. Interrupt-
source overrides are recorded but not applied until a real routing/handler
contract exists.

## 4. Why this is not yet SMP

Starting an AP requires more than enumerating it. The kernel still needs a
low-memory real-mode trampoline, INIT/SIPI sequencing, per-CPU stacks and
state, a controlled handoff into long mode, and an online handshake. Interrupt
entry must also become per-CPU safe before IPIs or scheduling are enabled.
Finally, the physical allocator and shared kernel state need synchronization.

## 5. Verification

After booting a QEMU image, run:

- `smp` — RSDP/MADT addresses, discovered and online CPU counts, BSP APIC ID,
  Local APIC state, interrupt-source override count, and masked I/O APIC entries.
- `irq` — verify the legacy PIT path still advances.
- `memtest` and `vmtest` — ensure the existing memory paths still pass.

Expected at this stage: online CPU count is one, even when QEMU presents more
than one CPU. Do not interpret the discovered count as an SMP success test.
