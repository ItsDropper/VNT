# Benchmark honestly

## Goal

Separate allocator cost from startup and noise.

## Understand the idea

Launching a process can dominate tiny benchmarks. Use many operations per run, warm up, repeat, and compare medians on the same machine.

## Do it

Keep correctness checks outside the timed region but run them before trusting timings.

## Exercise

Record compiler revision, optimization settings, operation count, and repeated timings.

## Checkpoint

Name three conditions for a fair comparison.

Before moving on, record the invariant and evidence. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
