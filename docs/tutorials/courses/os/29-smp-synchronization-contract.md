# Lesson 29 — SMP safety: synchronization before shared memory

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository root `LICENSE`.

## Goal

Explain why the APIC IPI test is not permission to run ordinary kernel work concurrently, and establish the exact prerequisites for the next kernel milestone: a VNT-native atomic primitive and a tested spinlock. This lesson audits the real code and defines checks that must pass before allocator calls can be made from multiple CPUs.

This is deliberately a source-audit lesson, not a claim that VNTOS already has atomic operations or a working spinlock. Do not copy a C atomic API or assume that an arbitrary inline-assembly string has compiler memory-ordering semantics.

## 1. The current execution model

Start with these files:

- `os/kernel/smp.vnt`: AP startup, online bookkeeping, IPI delivery test, and the vector-240 handler.
- `os/kernel/sync.vnt`: the initial spinlock API and its single-CPU self-test.
- `os/kernel.vnt`: initialization order and the IPI self-test.
- `os/arch/x86_64/interrupts.S`: the minimal IDT entry/exit boundary.
- `os/mm/phys.vnt`: page-frame bitmaps and cached allocation counters.
- `src/compiler/freestanding_x86.c`: code generation for the freestanding VNT subset.
- `.github/workflows/vntos-boot.yml`: the authoritative QEMU boot regression test.

The BSP starts APs serially. Each AP gets a separately allocated stack page, enters VNT, publishes its online state, enables interrupts, and idles with `hlt`. The BSP then sends one IPI at a time and waits for the corresponding response counter. The handler increments a counter and acknowledges the Local APIC.

The IPI test proves that a remote CPU can take an interrupt and return. It does **not** prove that two CPUs can safely allocate memory, update a shared bitmap, schedule tasks, or execute the same service concurrently.

## 2. The race in the physical allocator

Read `phys_alloc_page()` in `os/mm/phys.vnt`. It scans for a frame whose usable bit is set and allocated bit is clear, then sets the allocated bit and decrements the shared free-page count.

Those operations are a compound invariant, not one indivisible operation:

1. CPU A reads a frame's allocated bit as clear.
2. CPU B reads the same bit as clear.
3. Both select the same frame.
4. Both set the bit and both may decrement the free count.
5. Both callers receive the same physical address.

A similar lost-update race exists in bitmap byte writes: `phys_bitmap_set()` reads a byte, changes one bit in a local value, then writes the byte back. Two CPUs changing different bits in that byte can overwrite each other's updates. Protecting only the cached counter would not fix either problem.

`phys_free_page()` has the same synchronization requirement. Validation, clearing the allocation bit, and incrementing the count must be serialized as one transaction. Double-free rejection on one CPU is not sufficient when another CPU can allocate or free concurrently.

## 3. Why the current state accessors are not a lock

The kernel's shared scalar state is backed by storage defined in architecture assembly and accessed through compiler-generated inline assembly in VNT. A read followed by an increment and a write is still a read-modify-write sequence. The fact that each operation is a single machine instruction does not make the sequence atomic.

Likewise, `volatile`-like access, disabling interrupts on one CPU, or placing a memory barrier around ordinary loads and stores would not by itself provide mutual exclusion between CPUs. Local interrupt masking can prevent local interrupt re-entry; it cannot stop a different processor from entering the same critical section.

Before adding a lock, inspect the actual freestanding backend and its inline-assembly lowering. The compiler must expose an operation with a documented contract, and generated assembly must demonstrate that the operation is emitted as intended. Do not invent a VNT atomic syntax or silently move the lock algorithm into a new `.S` file.

## 3A. Backend audit: what is and is not available

The current freestanding backend recognizes a closed set of unsafe architecture calls in `src/compiler/freestanding_x86.c`. The existing raw-memory operations are emitted as ordinary `mov` instructions; `cpu_invlpg`, CR2 reads, and MSR access each have explicit lowering. The target-specific `cpu_atomic_exchange_u64(address, value)` primitive is now implemented and covered by positive and negative compiler fixtures. Compare-and-exchange is still not implemented.

The backend also emits inline assembly text directly from the IR node. That interface does not declare input/output operands, register clobbers, or a compiler memory clobber. Therefore, a line such as `asm volatile("lock; ...")` is not a safe substitute for a first-class atomic operation: the compiler cannot infer the memory effects or register constraints from an arbitrary text string.

The implemented operation has a fixed signature rather than opening a general inline-assembly escape hatch:

- It requires an aligned, valid pointer to a 64-bit storage location in the kernel's mapped address space.
- It emits x86-64 `xchgq` against memory, which is implicitly locked when the operand is memory.
- It returns the value observed before the exchange.
- It provides the x86 target's full ordering for this operation; this is a target-specific primitive, not a claim to implement a portable C memory model.
- It can be called only from an unsafe context, consistently with other raw-memory operations.
- A compiler fixture checks the emitted assembly, and a negative fixture checks the unsafe-context restriction.

This operation alone would not make the allocator safe. A spinlock still needs a carefully specified lock state, acquire/release behavior, interrupt-context policy, and a real multi-CPU contention test. Do not treat a lock as validated based only on a grep assertion for `xchgq`.

## 4. Requirements for the next implementation

The next implementation must keep its scope narrow and prove the lock under contention before integrating it into shared kernel subsystems:

- Keep the atomic-exchange contract explicit: aligned mapped `u64` pointer, old value returned, unsafe-only use, and x86 `xchgq` semantics.
- Specify failure behavior and the memory-ordering guarantees; x86 instruction ordering does not excuse an undocumented compiler contract.
- Stress-test the initial VNT spinlock under contention. The lock state must have a stable, aligned storage location and a known initialization value.
- Keep the critical section short. Never wait for an IPI, print to the console, or sleep while holding a spinlock.
- Define interrupt interaction before using the lock from interrupt context. A CPU must not deadlock by taking a lock in ordinary code and then being interrupted by a handler that tries to take that same lock.
- Only then protect allocator bitmap changes and cached counts as one transaction.

Do not enable general concurrent allocation merely because the lock compiles. CI now checks the emitted `xchgq` instruction and rejects use outside `unsafe { ... }`. The shell's `synctest` covers only uncontended acquire/try-acquire/release/double-release behavior. A QEMU test must still exercise contention from more than one CPU, detect duplicate allocations, and verify that the free-page count returns to its starting value.

## 5. Verification available today

From the repository root, the normal project commands are:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -smp 4 -serial stdio
```

In the existing shell, run `mem`, `memtest`, `vmtest`, and `smp`. These validate the current single-core allocator paths, virtual-memory probe, topology, and IPI counters. They are not a concurrency test.

The GitHub Actions workflow `VNTOS Boot and Serial Shell` builds the freestanding kernel and boots four virtual CPUs. Treat its exact result as the evidence; source inspection alone is not a build or boot result. Do not run a supposed two-CPU allocator stress test until the atomic primitive and lock implementation actually exist.

## 6. Completion criteria

The synchronization milestone is complete only when all of these are true:

- The atomic primitive is implemented in the compiler/backend with a documented contract and a regression fixture.
- Generated assembly is checked in CI for the expected atomic instruction and operand behavior.
- The VNT spinlock has tests for uncontended acquisition, contention, release, and lock-state invariants.
- A four-CPU QEMU test repeatedly allocates distinct pages from concurrent CPUs, frees them, and verifies accounting.
- Existing boot, shell, IPI, `memtest`, and `vmtest` checks still pass.

Until then, keep APs restricted to their current idle/IPI role. That is a deliberate safety boundary, not a missing feature to bypass with assembly.

---

[← Previous lesson](./28-apic-ipi-service.md) · [Course index](./README.md) · [Next lesson →](./30-physical-allocator-next-fit.md)
