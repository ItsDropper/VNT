# UDP datagrams

## Goal

Parse ports, length, and checksum rules.

## Understand the idea

UDP has source port, destination port, length, and checksum. The length field must be consistent with the available packet payload; IPv4 permits a zero UDP checksum in some cases.

## Do it

Create a UDP parser that returns fields only after validating the header and length. Keep IPv4 validation separate.

## Exercise

Construct a datagram and round-trip ports, payload, and length through parser and builder.

## Checkpoint

Why does a valid UDP header not guarantee an application payload is well-formed?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
