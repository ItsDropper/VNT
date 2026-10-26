# Fuzzing parsers

## Goal

Try adversarial byte arrays without network access.

## Understand the idea

Parser fuzzing means feeding many deterministic byte arrays and checking that the program rejects invalid inputs without out-of-bounds access or hangs.

## Do it

Generate bounded arrays of varied lengths and contents. Do not use raw sockets or transmit the generated packets.

## Exercise

Run hundreds of deterministic cases through Ethernet, IPv4, and UDP parsers; record crashes and unexpected acceptance.

## Checkpoint

What properties can a fuzz test verify even without knowing the correct parse result for every input?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
