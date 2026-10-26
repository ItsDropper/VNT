# Your first reservation

## Goal

Reserve and inspect bytes safely

## Understand the idea

Byte operations are the easiest first test because they avoid mixing in integer endianness and alignment assumptions.

## Do it

Reserve a small region, write known bytes using documented mem helpers, and read them back.

## Exercise

Keep allocator code in src/arena.vnt and demonstration flow in src/main.vnt. Check each write result.

## Checkpoint

Why is checking a write result part of correctness?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
