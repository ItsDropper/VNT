# Security and design review

## Goal

Audit ownership, overflow, and failure paths.

## Understand the idea

Review overflow, alignment, stale pointers, double destroy, import paths, and APIs that bypass validation. This is not a kernel heap or production allocator.

## Do it

State unsupported behavior honestly: no individual free, concurrency control, coalescing, or guard pages.

## Exercise

Run tests and document one limitation that matters in a real kernel.

## Checkpoint

What must change before multiple threads can share the arena?

Before moving on, record the invariant and evidence. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
