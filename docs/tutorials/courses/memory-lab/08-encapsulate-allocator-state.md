# Encapsulate allocator state

## Goal

Separate state representation from public operations

## Understand the idea

A global cursor makes multiple arenas interfere and tests order-dependent. A narrow module API keeps callers from bypassing invariants.

## Do it

Expose initialize, reserve, reset, inspect, and destroy. Use only syntax supported by this compiler.

## Exercise

Refactor main.vnt so it calls exported functions and performs no pointer arithmetic.

## Checkpoint

Name two invariants every public operation must preserve.

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
