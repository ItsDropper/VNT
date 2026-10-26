# Start here

## Goal

Build a safe, reproducible lab and define the finish line

## Understand the idea

An arena owns one backing allocation and reserves slices from it. Unlike a general allocator, it normally releases all reservations together. This course builds an arena allocator and inspector across separate source files.

## Do it

Read docs/raw-memory.md and inspect lib/std/mem.vnt. Use hosted programs only; never write to physical memory, kernel addresses, or arbitrary pointers.

## Exercise

Create examples/courses/memory-lab and record the compiler revision and expected output.

## Checkpoint

Explain pointer, allocation, offset, and owned region.

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
