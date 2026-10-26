# ARP and address resolution

## Goal

Map IPv4 addresses to simulated MAC addresses.

## Understand the idea

ARP resolves a local IPv4 address to a link-layer address. In a simulator, model the cache and request/reply messages as data; do not broadcast on a real network.

## Do it

Define an ARP cache with explicit hit, miss, insert, and expiry behavior. Keep the cache separate from frame parsing.

## Exercise

Simulate a cache miss, request creation, reply handling, and a later cache hit.

## Checkpoint

Why is ARP a link-local mechanism rather than an Internet-wide directory?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
