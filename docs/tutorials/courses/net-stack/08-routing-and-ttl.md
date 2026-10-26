# Routing and TTL

## Goal

Choose a route without pretending to reach the Internet.

## Understand the idea

Routing chooses a next hop based on destination prefixes. A toy lab can start with a small static table and longest-prefix match; it should not claim full router behavior.

## Do it

Represent routes as destination prefix, prefix length, and next-hop identifier. Decrement TTL only when forwarding and reject expiry.

## Exercise

Add two overlapping routes and prove the more-specific prefix wins.

## Checkpoint

What does longest-prefix match mean?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
