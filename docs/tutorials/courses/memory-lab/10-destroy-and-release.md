# Destroy and release

## Goal

Release storage once and invalidate the owner

## Understand the idea

Free ends the allocation's lifetime. Copies of a pointer elsewhere are not automatically cleared when the owner's pointer is nulled.

## Do it

Clear capacity and cursor as well as releasing storage. Define repeated-destroy behavior.

## Exercise

Ensure successful paths destroy the arena and failed initialization stays harmless.

## Checkpoint

Why can't nulling one pointer invalidate copies elsewhere?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
