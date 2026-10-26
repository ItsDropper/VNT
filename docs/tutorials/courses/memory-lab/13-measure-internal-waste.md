# Measure internal waste

## Goal

Quantify padding and unused capacity

## Understand the idea

An arena avoids individual-free fragmentation but still wastes alignment padding and unused tail capacity. Define what used bytes means.

## Do it

Track payload separately from cursor movement and validate cursor<=capacity before subtraction.

## Exercise

Report capacity, consumed bytes, payload bytes, and remaining bytes.

## Checkpoint

Why is 'used bytes' ambiguous?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
