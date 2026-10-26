# Checksums and one's complement

## Goal

Implement and test the Internet checksum.

## Understand the idea

IPv4 and ICMP use a one's-complement checksum over 16-bit network-order words. Odd-length data is padded with a zero byte for calculation; the final result is complemented after carry folding.

## Do it

Implement the checksum in a standalone module and verify it against known test vectors. Do not guess expected results from your own implementation.

## Exercise

Test empty input, odd length, carry folding, and a header whose checksum validates to zero under the chosen API convention.

## Checkpoint

Why does byte order matter when combining checksum words?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
