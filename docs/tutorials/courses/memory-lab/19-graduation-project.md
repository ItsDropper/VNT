# Graduation: build a multi-region memory inspector

## Goal

Combine allocator into a multi-file project.

## Understand the idea

Build an inspector with two arenas, a reservation log, and requested-versus-consumed reporting. Keep policy, reporting, entry point, and tests separate.

## Do it

Report offsets rather than raw addresses. Never inspect memory outside a live reservation; destroy arenas before exit.

## Exercise

Complete the scaffold, add deterministic tests, and document build commands and limitations.

## Checkpoint

Trace a request from size/alignment through validation to reservation.

Before moving on, record the invariant and evidence. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).
