# Sequence numbers and acknowledgements

## Goal

Track ordered bytes in a toy stream.

## Understand the idea

TCP sequence numbers count bytes, with SYN and FIN consuming sequence space. Real TCP includes wraparound arithmetic and many edge cases.

## Do it

For the simulator, use a clearly documented bounded integer model and reject out-of-order segments rather than pretending to support full TCP.

## Exercise

Add a test for expected sequence, duplicate segment, and a gap in the stream.

## Checkpoint

Why must a duplicate segment not append the same payload twice?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
