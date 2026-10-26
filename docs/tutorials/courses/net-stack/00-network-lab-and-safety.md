# Network lab and safety

## Goal

Build an offline packet laboratory.

## Understand the idea

A network stack is a sequence of parsers and state machines that turn bytes into protocol messages. This course implements a simulator, not a raw-socket tool: every packet is a byte array created and consumed by your own program.

## Do it

Read the repository language and array APIs. Keep the lab offline; do not inject crafted packets onto real networks.

## Exercise

Create examples/courses/net-stack with separate packet, checksum, IPv4, UDP, and demo modules.

## Checkpoint

Why is an offline simulator a better first project than sending raw packets?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
