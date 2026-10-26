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

## Work out the divisor

The conventional PIT input frequency is approximately 1,193,182 Hz. Dividing it by 100 gives about 11,931.82. The implementation uses divisor 11,931, which gives a frequency close to 100 Hz:

`1,193,182 / 11,931 ≈ 100.01` ticks per second.

This is an approximate rate. The PIT's real oscillator tolerance and emulator behavior mean that the counter is not a high-precision wall clock. A kernel wanting elapsed time should track calibrated time units and account for the timer's actual behavior.

## The PIC mask is a bit field

The master mask `0xfe` is binary `11111110`. Bit 0 is clear, so IRQ0 is enabled; bits 1–7 remain set and masked. The slave mask `0xff` is binary `11111111`, so all slave IRQs remain masked.

This detail is deliberate. The current PS/2 driver reads the controller in the main polling loop. If IRQ1 were unmasked without changing that design, both the IRQ handler and the polling loop could try to consume the same controller data byte. Correct interrupt-driven input needs a clear ownership rule and usually a queue between the handler and the shell.

## EOI and repeated delivery

The PIC expects an end-of-interrupt (EOI) command after a hardware IRQ has been handled. The common assembly path sends EOI to the master PIC for IRQs 32–39 and to both slave and master for IRQs 40–47. Only IRQ0 is enabled now, but the common path covers the mapped range.

An exception is not a PIC IRQ and must not receive a PIC EOI. That is why the vector-range check exists. Confusing exceptions and IRQs can create hard-to-diagnose behavior.

## Observe, then reason

Use the `irq` command twice with a delay. Record the displayed values and subtract the earlier value from the later value. The difference should be positive under normal QEMU operation. It is a count of delivered interrupts, not a timestamp. Do not infer exact seconds from the difference unless you account for the actual elapsed delay and timer rate.

If the value does not change, inspect the sequence in this order: IDT loaded, PIC remapped, PIT programmed, IRQ0 unmasked, CPU IF enabled, vector 32 stub installed, dispatcher increments the counter, and EOI sent. Changing several of these at once destroys useful evidence.

## The interrupt lifecycle from device to return

For the current timer path, each layer has a distinct responsibility:

1. PIT channel 0 reaches its programmed countdown and requests IRQ0.
2. The master PIC delivers the unmasked IRQ to the CPU.
3. The CPU consults IDT vector 32 and enters `isr32`.
4. The stub adds the vector/error fields and jumps to `interrupt_common`.
5. The common entry preserves registers and calls `interrupt_dispatch(32, 0)`.
6. VNT increments the shared tick count and returns quickly.
7. Assembly sends the EOI to the master PIC.
8. The common entry restores registers, removes its synthetic fields, and executes `iretq`.
9. The interrupted instruction stream continues.

This list is a debugging map. If the tick count never changes, inspect each transition rather than assuming that "the timer is broken."

## Why this is not yet a clock service

A clock service needs a defined time unit, a monotonic counter, a conversion from ticks to elapsed time, overflow behavior, and often calibration against a more stable source. The current `irq` command only exposes an interrupt count. It cannot tell you the current date or promise exact milliseconds.

A scheduler is a different service again. The current handler returns to the same interrupted context; it does not choose another task. A future preemptive scheduler must save and restore task context and make the interrupt return frame correspond to the selected task. Merely incrementing a timer counter does not create preemption.

## Hardware caveats

The PIT and legacy PIC are chosen for an educational baseline. Modern PCs often use APIC-family interrupt controllers and different timer sources. The exact same high-level VNT policy can be retained while replacing the hardware-routing layer, but the vector setup, EOI operation, timer initialization, and device masking rules would change.

Do not copy the PIC port writes into a driver for a different controller. First identify the controller and its required initialization sequence, then put that hardware-specific sequence behind a narrow interface.

## Why the counter is shared across modules

The storage is declared in `os/boot.S` because that file already owns the initial scalar state layout. The VNT dispatcher does not allocate memory for each tick; it updates the existing slot through `state_set`. The shell later reads that same slot through `state_get`. This is a minimal shared-state contract, and a future cleanup should replace numeric indexes with a documented state interface.

When documenting your measurements, include the QEMU command, the two hexadecimal counter readings, the approximate delay, and whether the kernel was otherwise idle. That makes the observation repeatable and prevents a counter value from being mistaken for a precise clock.
If the counter is unchanged, attach the transcript to your issue or review rather than reporting only a conclusion.

## 15. Make the timer test meaningful

A test that only finds the text `irq` in the console proves that the shell accepted the command and printed something. It does not necessarily prove that the interrupt was delivered. A stronger test reads the numeric counter, waits for a defined interval, reads it again, parses both values, and asserts that the later value is greater.

Be careful about the test environment: if the QEMU monitor sends input differently from the serial console, the test should state which input path it covers. A timer test should also avoid assuming an exact count unless the elapsed time and timer behavior are controlled.

## 16. Document what the counter can and cannot mean

The counter is a diagnostic count of delivered timer interrupts. It is not wall-clock time, a calibrated duration, a sleep API, or a scheduling metric. If later code uses the counter for elapsed time, define wraparound behavior and the conversion to time units. If a scheduler uses timer ticks, it additionally needs a safe context-switch contract and task state; the counter alone is not enough.
