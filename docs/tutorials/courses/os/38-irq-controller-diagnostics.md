# Distinguish IRQ delivery counts from controller configuration

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

Interrupt diagnostics must not confuse three different facts:

1. **Configuration:** which interrupts the controller currently masks.
2. **Delivery accounting:** how many legacy PIC vectors reached the VNT dispatcher.
3. **Device health:** whether a device is present and operating correctly.

The first two can be inspected by this early kernel. The third cannot be inferred from an interrupt counter alone.

## Read the live PIC mask

Open `os/pic.vnt` and find `pic_irq_is_masked(irq)`. The function validates the IRQ range, reads the master PIC data port (`0x21`) for IRQs 0–7 or the slave PIC data port (`0xA1`) for IRQs 8–15, shifts the selected bit to bit zero, and returns it. A result of 1 means masked; 0 means unmasked; -1 means the requested IRQ number is invalid.

This is a read-only diagnostic. It does not enable or disable any interrupt. The implementation uses the same early-kernel inline-assembly calling convention as other routines in this module, so any future compiler ABI change must review its stack offsets.

## Extend the `irq` command

In `os/kernel/shell.vnt`, the `irq` command reports:

- PIT timer ticks and the recorded legacy IRQ0/IRQ1 vector counts.
- The live PIC mask state for IRQ0 and IRQ1.
- Whether the Local APIC was initialized.
- Whether the supported I/O APIC masking setup completed.
- The count of I/O APIC redirection entries masked by that setup.

These values answer different questions. For the current setup, IRQ0 is expected to be unmasked and increase with timer delivery; IRQ1 is expected to be masked because keyboard input is polled. The I/O APIC readiness flag is important because a zero route count can mean setup did not complete, not that a controller has zero entries.

The I/O APIC routine intentionally masks routes rather than enabling unimplemented device handlers. Do not treat an initialized controller as proof that the kernel routes device interrupts correctly.

## Regression checks

The QEMU workflow invokes `irq` and checks that every new diagnostic label is followed by decimal digits. That verifies the command emitted a value, not that a particular value is correct on every machine. Firmware tables, emulated hardware, and controller initialization paths can differ.

A stronger future test could assert the expected mask values only in a QEMU configuration whose PIC setup is controlled and deterministic. Avoid changing PIC masks merely to test them: doing so could disable the timer or expose a device interrupt with no handler.

## Limits

- The delivery counters count vectors that reach `interrupt_dispatch`; they do not count device events that were lost before delivery.
- The counter storage is simple shared scalar state and is not an SMP-safe atomic accounting system.
- IRQ1 remains masked and PS/2 keyboard/mouse input is polled.
- The I/O APIC setup supports only its currently checked MMIO address and one-controller case.
- APIC readiness is a recorded initialization result, not a guarantee that every APIC feature works.

## Exercises

1. If IRQ1 is masked and its count remains zero, what can you conclude—and what can you not conclude?
2. Why should the `irq` command report both I/O APIC readiness and masked-entry count?
3. Explain why a timer count is not a calibrated wall clock.
4. Design a test for `pic_irq_is_masked` that does not change the hardware mask.
5. Identify what synchronization would be needed before treating shared IRQ counters as reliable across CPUs.

## Acceptance checklist

- [ ] PIC mask inspection is read-only and rejects IRQ numbers outside 0–15.
- [ ] The shell reports controller state separately from delivery counts.
- [ ] CI checks the presence and decimal formatting of the new diagnostics.
- [ ] Documentation does not equate a configured controller with a healthy device.
- [ ] No device IRQ is unmasked without a real handler and ownership model.

---

[← Previous lesson](./37-decimal-digit-self-test.md) · [Course index](./README.md)
