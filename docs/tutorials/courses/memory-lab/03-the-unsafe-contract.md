# The unsafe contract

## Goal

Keep risky operations small and reviewable

## Understand the idea

An unsafe block exposes risk but does not prove bounds, lifetime, ownership, alignment, or aliasing. The programmer must establish each precondition.

## Do it

Inspect exact signatures in lib/std/mem.vnt and docs/raw-memory.md.

## Exercise

Write a checklist: allocation succeeded, range validated, lifetime active, ownership clear, no use after free.

## Checkpoint

Name one bug unsafe blocks do not prevent.

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
