# ICMP messages

## Goal

Explain network errors as protocol data.

## Understand the idea

ICMP carries diagnostic/control messages such as echo request/reply and destination unreachable. A simulator can generate an echo reply without operating system privileges.

## Do it

Validate the ICMP type-specific minimum length and verify its checksum before handling a message.

## Exercise

Implement echo request to echo reply, changing type/code and recomputing checksum.

## Checkpoint

Why is an ICMP echo reply not proof that every service on a host is reachable?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
