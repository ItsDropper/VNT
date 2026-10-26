# Protocol errors and observability

## Goal

Make malformed packets diagnosable.

## Understand the idea

A robust parser returns structured errors such as truncated, invalid version, invalid length, checksum mismatch, or unsupported protocol. One generic 'bad packet' makes debugging needlessly hard.

## Do it

Separate parse errors from protocol decisions. Add counters for accepted, rejected, and unsupported packets.

## Exercise

Create one test per error category and print stable diagnostic names.

## Checkpoint

Why should malformed input fail closed rather than be partially accepted?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
