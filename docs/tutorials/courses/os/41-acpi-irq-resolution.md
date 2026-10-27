# Resolve ACPI ISA interrupt overrides

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

Learn how legacy ISA IRQ numbers map to Global System Interrupts (GSIs) on an ACPI system, and add a read-only API that resolves those mappings without enabling any hardware route.

## The problem: IRQ is not always GSI

Legacy device code commonly names IRQ0 through IRQ15. An I/O APIC, however, routes GSIs. The ACPI Multiple APIC Description Table (MADT) can contain Interrupt Source Override (ISO) entries that change the GSI associated with an ISA IRQ and specify electrical polarity and trigger mode.

For example, firmware commonly maps legacy IRQ0 away from GSI 0. Code that assumes every IRQ number is its GSI can therefore program the wrong route. The source IRQ and destination GSI must be kept conceptually separate.

## Implementation

The MADT parser in `os/firmware/acpi.vnt` already records up to sixteen override entries in three state slots per entry:

- source ISA IRQ;
- destination GSI;
- raw 16-bit flags.

The new `acpi_irq_gsi(irq)` function validates the legacy IRQ range (0–15), searches recorded overrides, and returns the matching GSI. If there is no override, it returns the conventional identity mapping, `GSI = IRQ`. It returns `-1` for invalid input and `-2` when duplicate source entries make the result ambiguous.

`acpi_irq_override_flags(irq)` follows the same lookup policy. It returns zero when no override exists, the raw flags when one matching override exists, `-1` for invalid input, and `-2` for duplicate entries. Returning raw flags keeps this change narrowly scoped: callers must not treat a nonzero value as a decoded polarity or trigger mode.

The shell's `irq` command now prints the effective GSI and raw override flags for IRQ0 and IRQ1. The boot workflow checks that these fields are present and followed by decimal output.

## Safety boundary

These functions only resolve firmware data. They do not write I/O APIC registers, unmask a route, or enable a device interrupt. That distinction is deliberate: route setup must separately validate the I/O APIC's GSI range, decode the ACPI polarity/trigger encodings, choose a vector, install a handler, and define interrupt ownership before enabling delivery.

The current kernel still masks I/O APIC routes. Do not remove that safety policy merely because the lookup functions return plausible values.

## Exercises

1. Explain why the source IRQ and destination GSI are separate numbers.
2. What should the resolver return if two ISO entries have the same source IRQ, and why is silently choosing the first unsafe?
3. Why does a default mapping return flags zero?
4. List the additional checks required before a future driver can enable an I/O APIC route.

## Acceptance checklist

- [ ] IRQ arguments outside 0–15 are rejected.
- [ ] A single matching ISO entry takes precedence over the identity mapping.
- [ ] Missing overrides use the conventional identity mapping and zero flags.
- [ ] Duplicate source overrides are reported as ambiguous.
- [ ] Shell diagnostics expose IRQ0/IRQ1 effective GSIs and raw flags.
- [ ] CI checks the output labels without assuming a particular firmware's override table.
- [ ] No I/O APIC route is enabled by this change.

---

[← Previous lesson](./40-smp-atomic-counter-self-test.md) · [Course index](./README.md)
