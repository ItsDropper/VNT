# Overflow-safe bounds checks

## Goal

Prove a reservation fits before changing state

## Understand the idea

The check offset+size<=capacity can overflow. After proving offset<=capacity, check size<=capacity-offset. Validate alignment arithmetic separately.

## Do it

Calculate candidates first and update the cursor only after every check passes.

## Exercise

Implement validation whose failure path leaves state unchanged.

## Checkpoint

How can an early cursor update corrupt future allocations?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
