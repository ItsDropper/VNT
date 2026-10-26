# Testing and performance

## Goal

Measure the simulator without hiding correctness failures.

## Understand the idea

Correctness comes first: test vectors and malformed-input tests must pass before timing. A microbenchmark should process many packets per run and report repeatable measurements.

## Do it

Use the repository's existing test conventions and keep deterministic input fixtures. Measure parser throughput only after verifying outputs.

## Exercise

Add a fixed packet corpus and report packets processed, rejects, and elapsed time without claiming production network throughput.

## Checkpoint

Why is a fast parser useless if it accepts malformed lengths?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
