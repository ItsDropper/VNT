# Ethernet frames

## Goal

Separate link-layer framing from network protocols.

## Understand the idea

An Ethernet II frame has destination MAC, source MAC, EtherType, then payload. The simulator does not need a physical NIC to teach framing.

## Do it

Represent MAC addresses as six bytes and reject frames shorter than the minimum header length used by your parser.

## Exercise

Build and parse a sample frame and confirm source, destination, and EtherType round-trip.

## Checkpoint

Why should the Ethernet parser not interpret IPv4 fields itself?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
