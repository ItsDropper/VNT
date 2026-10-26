# Fixed-width values

## Goal

Store integers and understand byte order

## Understand the idea

read_u32 and write_u32 make access width explicit but do not add a full unsigned type system. x86-64 uses little-endian byte order.

## Do it

Reserve enough bytes, write a known value, and read it with the matching accessor.

## Exercise

Compare fixed-width round-trip with individual bytes and note signedness limits.

## Checkpoint

What does endianness describe?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
