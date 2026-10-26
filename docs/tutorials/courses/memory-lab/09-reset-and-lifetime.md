# Reset and lifetime

## Goal

Understand why reset invalidates old reservations

## Understand the idea

Reset can rewind the cursor without releasing storage. Old addresses may remain mapped, but the caller no longer owns those reservations logically.

## Do it

Finish using every reservation before reset. Never read a stale pointer just to see if it works.

## Exercise

Demonstrate a new allocation after all old uses have ended.

## Checkpoint

Why does an unchanged address not mean its original allocation is valid?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
