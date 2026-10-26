# Create a bounds and alignment test matrix

## Goal

Test edge cases, not just success.

## Understand the idea

Test exact fit, one byte too large, invalid alignment, padding overflow, reset, failed initialization, and repeated destruction.

## Do it

Keep tests independent. Use separate .vnt files if the runner treats each as a separate program.

## Exercise

Add exact-capacity and oversized-reservation tests with explicit failure markers.

## Checkpoint

Why is one successful-allocation test insufficient?

Before moving on, record the invariant and evidence. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
