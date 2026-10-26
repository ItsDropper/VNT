# Allocation failure

## Goal

Handle null before pointer arithmetic

## Understand the idea

Allocation can fail. Do not publish an arena as initialized until backing storage exists. Failed initialization must not leave half-valid state.

## Do it

Reject invalid capacity, allocate, check mem.is_null, then store the base and capacity.

## Exercise

Implement initialization as a staged operation with a documented failure result.

## Checkpoint

What state should remain after failed initialization?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
