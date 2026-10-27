# Debug memory failures

## Goal

Turn crashes into small reproducible cases

## Understand the idea

Memory errors can surface far from the invalid write. Reduce the reproducer and check operation results before changing several things.

## Do it

Use repository language checks and deterministic small inputs; a successful run is not proof of all bounds.

## Exercise

Request more than capacity and verify failure plus unchanged usage.

## Checkpoint

What evidence proves a failed reservation did not mutate state?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
