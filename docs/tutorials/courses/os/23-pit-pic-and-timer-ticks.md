# Lesson 23 — The PIT, PIC, and Timer Ticks

**Goal:** understand the first real hardware interrupt that VNTOS handles.

## Two separate hardware jobs

The **Programmable Interval Timer (PIT)** generates periodic timing events. The legacy **Programmable Interrupt Controller (PIC)** routes those events to the CPU. They solve different problems: programming the PIT does not install a handler, and installing an IDT gate does not cause a timer to tick.

VNTOS configures PIT channel 0 with divisor 11931, which is approximately 100 ticks per second using the conventional PIT input frequency. This is a teaching timer, not a precision clock. The kernel does not yet account for elapsed time, calibrate drift, or use the local APIC timer.

## The PIC setup sequence

In `vntos_interrupts_init` in `os/interrupts.S`, the code sends initialization commands to the master PIC at ports `0x20/0x21` and the slave PIC at `0xa0/0xa1`. It sets their vector offsets to `0x20` and `0x28`, describes their cascade wiring, and returns both controllers to normal operation.

After programming the PIT, it writes mask `0xfe` to the master PIC and `0xff` to the slave PIC. Bit 0 is cleared in `0xfe`, so IRQ0 is unmasked; all other IRQ lines remain masked. This is why the keyboard remains polled even though the timer uses interrupts.

Finally, `sti` enables maskable interrupts. It occurs only after the IDT and controller state have been initialized.

## What the tick counter means

The VNT dispatcher increments `vntos_state[11]` for vector 32. The `irq` shell command prints that counter. It counts delivered timer interrupts since the kernel initialized the timer; it is not a wall-clock timestamp.

The counter is useful for a simple proof: if it increases while the shell is idle, the IDT entry, assembly frame, VNT dispatcher, PIC acknowledgement, and timer configuration are working together at least well enough for repeated IRQ0 delivery.

## Do not overclaim

This implementation does not schedule tasks. The timer handler returns to the exact interrupted execution. There is no context switch, saved task object, quantum policy, or ready queue. A scheduler would require all of those concepts plus a deliberate stack-switching ABI.

The implementation also does not enable keyboard IRQ1. That should be a separate lesson after an interrupt-safe input queue exists; simply unmasking IRQ1 while the current polling driver also reads the controller would create competing consumers of the same device data.

## Checkpoint

Explain the role of the PIT, PIC, IDT, dispatcher, and EOI separately. Describe what would happen if the handler never acknowledged IRQ0.

## Exercise

Run VNTOS, enter `irq`, wait several seconds, and enter `irq` again. The hexadecimal counter should have increased. Record both values and explain why the difference is not a precise elapsed-time measurement.
