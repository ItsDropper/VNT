# Use atomic increments for shared interrupt counters

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Why ordinary read-modify-write is unsafe

A counter update written as `state_set(slot, state_get(slot) + 1)` consists of separate operations: load, add, store. If two CPUs execute that sequence at once, both can read the same old value and then store the same incremented value. One event disappears from the count.

Interrupt context adds another constraint: acquiring a normal spinlock can deadlock if an interrupt interrupts code that already holds that lock. Keep the accounting path short and avoid allocating memory or printing from the dispatcher.

## Add a narrowly scoped atomic primitive

In `os/kernel/core.vnt`, `state_increment_atomic(index)` emits an x86-64 locked increment against the aligned `vntos_state` array:

```vnt
fun state_increment_atomic(index: int) {
    unsafe {
        asm volatile("leaq vntos_state(%rip), %rax; movq -8(%rbp), %rcx; lock incq (%rax,%rcx,8)")
    }
    return 0
}
```

The `lock` prefix makes the memory update atomic between CPUs. The code uses this project's current inline-assembly argument convention, where the first argument is read from `-8(%rbp)`; that convention is compiler-specific and must be revisited if the freestanding ABI changes.

This is deliberately not a general atomic API: it accepts a state-slot index and relies on callers to use a valid slot. Keep it internal to known kernel counters rather than exposing arbitrary indexes to shell input.

## Apply it to timer and IRQ accounting

`kernel_timer_tick()` now increments slot 11 through the primitive. `device_irq_record(irq)` validates the legacy IRQ range 0–15 and atomically increments slot `12 + irq`. Invalid IRQ numbers still return without touching memory.

The dispatcher still does minimal work: record the vector and, for IRQ0, account for a timer tick. It does not print or run device handlers. The shell reads the counters as snapshots. On x86-64, aligned 64-bit loads are atomic, but a displayed value can become stale immediately after it is read.

## What this does—and does not—guarantee

- Competing atomic increments to the same slot no longer overwrite one another.
- It does not make all accesses to that slot safe if some future code uses ordinary read-modify-write updates.
- It does not make the whole `vntos_state` array, PS/2 drivers, memory allocator, or device ownership model SMP-safe.
- It does not prove the IRQ routing is correct or that all hardware events reach the dispatcher.
- The current QEMU transcript checks output labels, not a multi-CPU stress test of the counter primitive.

## Verification plan

The project's GitHub Actions workflow remains the authoritative build and QEMU test. Inspect the run for the commit that contains this change; do not infer success from the source being committed. A future dedicated test should run multiple CPUs against a dedicated counter, coordinate start/finish without printing in the hot path, and compare the final count against the exact number of increments. Do not reuse a live timer or IRQ slot for such a test.

## Exercises

1. Show a two-CPU interleaving where load/add/store loses an increment.
2. Explain why a spinlock used from an interrupt handler can deadlock.
3. Why is a counter read only a snapshot even when increments are atomic?
4. What additional validation would a general `state_increment_atomic` API need?
5. Design a QEMU stress test that counts increments from multiple CPUs without relying on wall-clock timing.

## Acceptance checklist

- [ ] Timer and legacy IRQ counters use atomic increments.
- [ ] IRQ input is range-checked before deriving the state slot.
- [ ] Interrupt dispatch remains allocation-free and does not print.
- [ ] Documentation limits the claim to the counters changed here.
- [ ] CI success is reported only after inspecting the actual workflow result.

---

[← Previous lesson](./38-irq-controller-diagnostics.md) · [Course index](./README.md)
