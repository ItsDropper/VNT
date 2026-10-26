# Deterministic stress testing

## Goal

Generate many requests reproducibly

## Understand the idea

A deterministic request sequence finds invariant failures while making them repeatable. Generate sizes and alignments, never arbitrary addresses.

## Do it

After each operation verify cursor bounds and non-overlap of successful regions.

## Exercise

Add hundreds of bounded attempts and print iteration on failure.

## Checkpoint

Why deterministic before random stress?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
