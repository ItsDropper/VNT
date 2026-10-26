# Build a packet cursor

## Goal

Centralize safe reads and writes.

## Understand the idea

A cursor tracks a byte buffer and current offset. Centralizing range checks avoids each protocol reimplementing unsafe indexing rules.

## Do it

Define read_u8, read_be16, write_u8, and write_be16 around the actual VNT array/indexing capabilities.

## Exercise

Add tests for reading the last valid field and rejecting a truncated field.

## Checkpoint

Why should failed reads leave the cursor unchanged?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
