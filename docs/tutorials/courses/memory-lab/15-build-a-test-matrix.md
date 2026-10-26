# Build a test matrix

## Goal

Test edge cases, not just success

## Understand the idea

Test exact fit, one byte too large, invalid alignment, padding overflow, reset, failed initialization, and repeated destruction.

## Do it

Keep tests independent. Use separate .vnt files if the current runner launches each test separately.

## Exercise

Add exact-capacity and oversized-reservation tests with explicit failure markers.

## Checkpoint

Why is one successful-allocation test insufficient?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
