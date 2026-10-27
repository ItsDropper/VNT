# Lesson 28 — Fixed-vector APIC IPIs

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository root `LICENSE`.

## Goal

Verify that an online secondary processor can receive a local-APIC fixed IPI, execute a VNT interrupt handler, acknowledge the APIC, and resume its idle loop.

## What changed

- The IDT now has a dedicated gate for vector 240.
- Secondary CPUs load the shared IDT before enabling their local APIC's spurious vector.
- AP-local LINT0/LINT1 and unused local-vector-table sources are masked so legacy PIC virtual-wire traffic is not delivered to an AP.
- The APs enter an interruptible `sti; hlt` loop after publishing online state.
- The BSP sends one fixed IPI to each online AP and waits for that CPU's response counter to change.
- The QEMU boot workflow uses four virtual CPUs and requires three AP responses.

## Handler contract

The vector-240 handler is deliberately short. It identifies the current APIC ID, increments that CPU's counter, sends a Local APIC EOI, and returns. It does not allocate memory, print to the console, schedule tasks, or touch the physical allocator.

## Verification

Boot the four-CPU QEMU image and look for `APIC IPI self-test: PASS. AP responses: 0x0000000000000003`. Run `smp` to inspect the online count and per-CPU IPI counters. A failed assertion means the boot workflow should be treated as failed; do not infer success from AP discovery alone.

## Limits

This is a cross-CPU interrupt-delivery test, not a scheduler or production SMP runtime. Per-CPU TSS/IST state, synchronized allocation, cross-core TLB shootdowns, interrupt-driven device routing, and context switching remain unimplemented. The shared IDT is read-only after initialization; APs are restricted to the idle loop and this handler.
