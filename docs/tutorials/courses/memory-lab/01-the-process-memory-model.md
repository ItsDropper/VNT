# The process memory model

## Goal

Distinguish virtual addresses from valid allocations

## Understand the idea

A process uses virtual addresses translated by the operating system and CPU. VNT ptr values do not carry bounds or ownership. A non-null address can still be invalid.

## Do it

Only access memory owned by a live allocation or another documented valid region. Keep base pointer and capacity together conceptually.

## Exercise

Represent base, capacity, and cursor in the arena module; check current compiler syntax before choosing a representation.

## Checkpoint

Why does non-null not prove safety?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
