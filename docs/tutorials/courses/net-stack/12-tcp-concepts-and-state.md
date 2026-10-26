# TCP concepts and state

## Goal

Model a connection as a state machine.

## Understand the idea

TCP is reliable, ordered byte-stream transport with sequence numbers, acknowledgements, retransmission, and congestion control. A beginner simulator must state which parts it does not implement.

## Do it

Create a state transition table for CLOSED, LISTEN, SYN-RECEIVED, ESTABLISHED, and CLOSED teardown. This is a teaching model, not production TCP.

## Exercise

Implement transition validation for a handshake using synthetic segments only.

## Checkpoint

Why is TCP much more than sending a SYN and receiving a SYN-ACK?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.
