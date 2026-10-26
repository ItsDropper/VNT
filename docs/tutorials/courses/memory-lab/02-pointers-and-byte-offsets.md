# Pointers and byte offsets

## Goal

Use offsets without losing bounds

## Understand the idea

mem.offset computes an address but does not validate it. Derive addresses only after checking integer offsets against the owning allocation.

## Do it

Keep offsets as allocator state; never derive pointers from untrusted integers without validation.

## Exercise

Add a helper that accepts offsets only between zero and capacity inclusive.

## Checkpoint

Why can capacity be an end marker but not a byte to read?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
