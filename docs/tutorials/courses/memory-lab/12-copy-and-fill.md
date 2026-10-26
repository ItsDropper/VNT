# Copy and fill

## Goal

Use bulk operations only on validated ranges

## Understand the idea

mem.copy is overlap-safe according to the documented API; mem.set fills bytes. Neither proves the supplied pointers and lengths are valid.

## Do it

Validate source and destination independently before calling helpers.

## Exercise

Fill one region, copy into another, inspect positions, and follow the actual zero-length contract.

## Checkpoint

Which risk does overlap-safe copying solve, and which remains?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
