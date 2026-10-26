# Fragmentation and reassembly

## Goal

Handle packet boundaries explicitly.

## Understand the idea

IPv4 fragments share an identification value and carry offsets in eight-byte units. Reassembly needs timeouts, consistency checks, and a memory cap to avoid unbounded buffering.

## Do it

Implement only a bounded teaching reassembler. Validate offsets and lengths before storing fragment bytes.

## Exercise

Reassemble two fragments in order and reject overlap or incomplete data according to your documented policy.

## Checkpoint

Why must a reassembler cap stored bytes and pending packets?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
