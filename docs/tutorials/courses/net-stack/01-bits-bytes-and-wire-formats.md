# Bits, bytes, and wire formats

## Goal

Read protocol fields as bytes.

## Understand the idea

Protocols define field widths, byte order, and layout. Never assume a packet is a native struct or that a buffer contains enough bytes.

## Do it

Use byte arrays and explicit offsets. Every parser must check length before reading a field.

## Exercise

Write a field table for a 20-byte IPv4 header: offset, width, meaning, and byte order.

## Checkpoint

What happens if a parser reads a field before checking packet length?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
