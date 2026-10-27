# Stress-test atomic increments across CPUs

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

Turn the atomic increment primitive from a source-level claim into a repeatable, multi-CPU kernel check. The `atomictest` shell command makes the bootstrap processor (BSP) and each online application processor (AP) increment one dedicated counter concurrently, then compares the final value with the exact expected total.

This is a bounded smoke test for the current x86-64/QEMU implementation. It is not a formal proof of all memory-ordering behavior, and it does not make unrelated kernel state SMP-safe.

## Why the earlier tests were insufficient

A counter can appear correct in a single-CPU test even when its update is implemented as separate load, add, and store operations. Two CPUs can read the same value and overwrite one another's increments. The `lock incq` instruction should prevent this lost-update behavior, but we need a test that actually creates overlapping work.

Do not run the test against the live timer or IRQ counters: those counters continue to change for unrelated reasons. Instead, the assembly-defined `vntos_state` array reserves three dedicated slots:

- Slot 308: the shared atomic counter.
- Slot 309: the test-armed flag.
- Slot 310: the number of AP handlers that completed their batch.

The slots are appended after the shell-history state, preserving the earlier slot numbers.

## How the test works

The implementation lives in `os/kernel/smp.vnt`, with the user-facing command in `os/kernel/shell.vnt`.

1. Require at least two online CPUs and require all discovered CPUs to be online. If this precondition is not met, report `SKIP`, not `PASS`.
2. Clear the dedicated counter and completion count, then arm the test.
3. Send fixed-vector IPI 240 (hexadecimal `0xf0`) to each online AP.
4. In the IPI handler, each AP that observes the armed flag performs 1,000 calls to `state_increment_atomic(308)`, then atomically increments the AP-completion count.
5. The BSP performs the same 1,000 increments while APs handle their IPIs.
6. Wait for the expected AP completions with a bounded spin loop, disarm the test, and compare the result against `online_cpu_count * 1000`.

The central operation is intentionally small:

```vnt
let work: int = 0
while work < 1000 {
    state_increment_atomic(308)
    work = work + 1
}
```

The shared counter must be incremented atomically by every participant. A plain `state_set(308, state_get(308) + 1)` would reintroduce the race the test is intended to detect.

## Run and interpret it

Boot the kernel with at least two CPUs. The repository's QEMU workflow uses four:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -smp 4 -serial stdio
```

At the VNTOS prompt, run:

```text
atomictest
```

For four online CPUs, the success line should report 4 CPUs, 1,000 increments per CPU, and a total of 4,000. A `FAIL` result reports the observed total and AP completion count. A `SKIP` result means the CPU topology does not satisfy the test preconditions; it is not evidence that the primitive passed.

The CI workflow runs this command under `-smp 4` and asserts the exact success transcript. The source change being committed is not itself proof that CI passed: inspect the GitHub Actions run for the resulting commit.

## Important limitations

- The test uses the current project-specific inline-assembly calling convention. The first argument to `state_increment_atomic` is read from `-8(%rbp)`; changing the compiler's freestanding ABI requires revisiting this primitive.
- The test assumes APs have completed startup and can receive the existing fixed-vector IPI. It deliberately skips if not all discovered CPUs are online.
- The test is run from the shell while the BSP has normal interrupt handling enabled. AP handlers perform bounded work and acknowledge the IPI after the batch.
- The armed flag is a simple test-control state, not a general synchronization primitive.
- Passing this test does not prove the physical allocator, shell history, CPU online count, spinlock implementation under contention, or arbitrary shared data are safe for concurrent access.
- A timeout or failed IPI is a failure, not a successful test with a smaller count.

## Exercises

1. Predict the expected counter total with 2, 4, and 8 online CPUs.
2. Explain why the AP-completion count is separate from the increment counter.
3. Change the batch size only after tracing every use of the constant, and update the CI expectation accordingly.
4. Describe how a broken load/add/store primitive could produce a total below the expected value.
5. Propose a follow-up test that repeats the workload many times without reusing a counter while a prior AP handler is still active.

## Acceptance checklist

- [ ] The test uses dedicated slots and does not mutate live timer/IRQ counters.
- [ ] Every CPU performs exactly 1,000 atomic increments.
- [ ] The BSP waits for all AP batches with a bounded loop.
- [ ] The final count is compared with the exact CPU-count-derived expectation.
- [ ] The test reports SKIP when the CPU topology is insufficient.
- [ ] CI asserts the expected four-CPU result and is checked for the actual outcome.

---

[← Previous lesson](./39-atomic-interrupt-counters.md) · [Course index](./README.md)
