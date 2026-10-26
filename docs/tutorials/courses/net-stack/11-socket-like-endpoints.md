# Socket-like endpoints

## Goal

Demultiplex datagrams to simulated services.

## Understand the idea

A protocol stack needs to decide which service receives a packet. Model endpoints as local address and port pairs rather than calling real sockets.

## Do it

Create a registry mapping destination ports to handlers. Define behavior for unbound ports and avoid silently dropping every error.

## Exercise

Register an echo service and deliver a UDP datagram to it in the simulator.

## Checkpoint

Why should the network parser not contain application-specific echo logic?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
