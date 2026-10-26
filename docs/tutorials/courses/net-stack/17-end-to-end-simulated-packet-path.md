# End-to-end simulated packet path

## Goal

Connect link, network, and transport layers.

## Understand the idea

A layered simulator should parse Ethernet, validate IPv4, dispatch UDP, run an application handler, then build the response in reverse order. Each layer owns its own invariants.

## Do it

Keep protocol modules independent and pass validated fields between them. Avoid a single giant function that mixes all layers.

## Exercise

Send a synthetic UDP echo request through the full pipeline and verify the response payload and checksums.

## Checkpoint

Which layer should reject a malformed IPv4 total length?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
