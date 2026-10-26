# Lesson 22 — Keep Interrupt Policy in VNT

**Goal:** trace an event from the assembly entry stub into ordinary VNT code and understand why handlers must stay small.

## The entry point

Open `os/interrupts.vnt`. Its `interrupt_dispatch(vector, error)` function is called by `interrupt_common` after the registers have been saved. The two parameters describe the normalized vector number and error code.

The dispatcher currently has three outcomes:

1. **Vector 32:** this is IRQ0 from the PIT timer. Increment the shared timer counter and return.
2. **Vectors below 32:** treat the event as a CPU exception, print the vector and error code, then halt.
3. **Other vectors:** treat them as unexpected in this kernel version, print the fallback vector marker, then halt.

This is intentionally a conservative policy. Continuing after an unknown CPU exception can corrupt state or hide the original bug.

## Why the timer handler does not print

The serial driver polls the UART until its transmitter is ready. Printing from interrupt context could keep the CPU inside the handler for a long time. It could also interleave output with the shell. The timer handler therefore updates one counter and returns quickly.

A future kernel should use a lock-free event buffer or another carefully designed logging path for interrupt-context diagnostics. Do not add allocation, blocking input, shell parsing, or lengthy console output to the timer path.

## Shared state is not a general concurrency solution

The tick count is stored in slot 11 of `vntos_state`, whose storage is reserved in `os/boot.S`. The shell reads it with the existing `state_get(11)` helper. This keeps the teaching implementation small, but it is not a general atomic counter API and the helpers still use compiler-specific inline assembly with hard-coded stack offsets.

That limitation matters: the module boundary is cleaner now, but it does not magically make the old low-level primitives safe. A real next step is to add explicit compiler-supported port-I/O, interrupt-control, and memory-access intrinsics and test their ABI.

## Walk the call chain

Trace these exact symbols:

```text
PIT fires
  -> isr32 in os/interrupts.S
  -> interrupt_common
  -> interrupt_dispatch in os/interrupts.vnt
  -> state_get/state_set in os/kernel.vnt
  -> interrupt_common sends PIC EOI
  -> iretq restores the interrupted execution
```

The assembly common path sends the end-of-interrupt signal after the VNT handler returns. Without acknowledging the PIC, the timer may stop generating further interrupts.

## Failure behavior

The fatal exception path prints a diagnostic and calls `cpu_halt()`. It does not attempt to recover, unwind a stack, continue a process, or reboot automatically. The current kernel has no process isolation or exception-recovery framework.

## Checkpoint

Why should an IRQ handler do as little as possible? Why is a single shared integer not enough to claim that the kernel has a complete synchronization design?

## Exercise

Read the timer branch and explain why it has no console calls. Then add a comment describing the limitation of the shared counter without changing its behavior.
