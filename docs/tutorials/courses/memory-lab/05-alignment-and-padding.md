# Alignment and padding

## Goal

Reserve addresses satisfying alignment

## Understand the idea

For power-of-two alignment a, align x with (x+a-1)&~(a-1), but check overflow first. Padding consumes capacity too.

## Do it

Reject zero and non-power-of-two alignment; calculate candidates locally.

## Exercise

Test aligned, misaligned, and padding-causes-failure cases.

## Checkpoint

Why can padding make a reservation fail?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
