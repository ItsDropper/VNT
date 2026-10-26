# IPv4 header parsing

## Goal

Validate the IPv4 header before payload access.

## Understand the idea

IPv4 includes version, IHL, total length, identification, flags/fragment offset, TTL, protocol, checksum, and addresses. IHL and total length are untrusted values.

## Do it

Check version, minimum header size, IHL bounds, and total length against the available bytes before returning a payload slice.

## Exercise

Reject truncated, malformed, and inconsistent-length headers with distinct error results.

## Checkpoint

Why can you not assume every IPv4 header is exactly 20 bytes?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
