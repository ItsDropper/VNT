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

## Handler design contract

Treat `interrupt_dispatch` as a restricted environment, even though it is written in VNT:

- It must not allocate memory; the kernel has no general heap.
- It must not wait for input.
- It must not parse shell commands.
- It should not print routine timer events.
- It must return quickly for the timer IRQ.
- For fatal exceptions, it should report the limited state it has and halt.

The dispatcher is not a task scheduler. It runs on the interrupted kernel stack and returns to the interrupted code after a normal IRQ. No task is selected, no saved stack is replaced, and no address space changes.

## Understand the fatal path

For a CPU exception, the VNT dispatcher prints the vector and error code through the existing console, then calls `cpu_halt()`. The current console combines VGA and COM1 output, so the report can be visible in QEMU's window and serial transcript. The handler does not decode every architecture-specific error bit yet.

Do not deliberately trigger a fault on a physical machine to test this. Once the exception handler is active, a controlled QEMU experiment can be used in a separate branch, but only after the normal boot and timer tests pass and you know how to reset the VM.

## Why timer IRQ code avoids console output

Imagine printing one line for every timer tick at about 100 Hz. A polling UART can spend a significant amount of time waiting for each character, and console output may overwrite text that the shell is writing. The interrupt handler would then do slow work at the exact moment it should be returning promptly to the interrupted code.

Instead, the handler increments the counter and leaves formatting to the `irq` shell command. This is the simplest form of deferred observation: collect a small fact in the handler and display it later in normal execution.

## Limitation worth fixing later

The shared `vntos_state` array is a compact teaching device, not a type-safe kernel data model. Index 11 is currently the timer count, but raw indices do not communicate ownership or atomicity. A future refactor should give timer state a named interface and decide whether reads require interrupts to be masked or a wider atomicity guarantee.

Keep the current implementation simple while learning the interrupt path; do not add a complicated synchronization layer before you can test it.

## Separate event classification from response

The dispatcher is intentionally simple, but its branches model a useful design rule: first classify the event, then apply the policy for that class. A timer IRQ is expected and returns quickly. A CPU exception indicates that an instruction could not proceed normally and is fatal in this kernel. An unexpected non-timer vector is also fatal because this version has no handler contract for it.

Do not merge all three cases into a single generic "print and return" branch. Returning from a page fault without fixing the faulting mapping would normally repeat the same fault. Returning from a general-protection fault can hide a broken kernel invariant. The conservative halt makes the failure visible instead of pretending to recover.

A later implementation can add a named exception-message table, but it must preserve the numeric vector and raw error code so the friendly name does not discard useful evidence. For page faults, the CPU's fault-address register must also be read through an architecture-specific primitive; it is not included in the current dispatcher arguments.
