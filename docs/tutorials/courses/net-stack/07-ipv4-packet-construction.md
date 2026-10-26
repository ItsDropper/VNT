# IPv4 packet construction

## Goal

Build a packet from validated fields.

## Understand the idea

Packet construction is the inverse of parsing: allocate the exact byte count, write fields in network byte order, calculate the header checksum, and verify total length.

## Do it

Keep field writing in one module. Use constants for header offsets and test every byte in the produced header.

## Exercise

Construct a minimal IPv4 packet and parse it back; compare all fields and payload.

## Checkpoint

Why must the checksum be computed after the header fields are finalized?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
