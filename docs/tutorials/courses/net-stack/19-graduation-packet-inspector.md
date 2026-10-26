# Graduation: packet inspector

## Goal

Build a multi-file protocol inspection tool from the modules in this course.

## Understand the idea

A useful inspector decodes supplied hex fixtures, prints validated Ethernet/IPv4/UDP fields, checks checksums, and explains rejection reasons. It should make protocol boundaries visible instead of hiding every layer in one parser.

## Do it

Keep the tool offline. Read fixtures from repository files or embedded test data; do not open raw sockets, sniff traffic, or send packets. Verify every parser against its documented length and checksum rules.

## Exercise

Complete the modules in `examples/courses/net-stack/`, add valid and malformed fixtures, and run an end-to-end synthetic UDP echo request through the simulator. Keep the entry point, packet cursor, checksum, IPv4, UDP, and reporting logic in separate files.

## Checkpoint

Trace one synthetic frame from Ethernet bytes to the UDP application payload and back. Explain which layer rejects a malformed IPv4 total length and how the test proves it.

Before completion, document unsupported behavior and run all offline tests. This is a learning simulator, not a production network stack.
