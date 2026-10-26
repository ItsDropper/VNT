# Lesson 20 — Interrupts, Exceptions, and the IDT

**Goal:** understand what happens when the CPU needs the kernel's attention and how VNTOS maps an event to code.

## Three terms that are not interchangeable

- An **exception** is raised by the CPU while executing an instruction, for example a divide error or page fault.
- A **hardware interrupt** is an asynchronous event from a device or interrupt controller. VNTOS currently enables the legacy PIT timer interrupt.
- A **software interrupt** is requested by an instruction such as `int`. VNTOS does not currently expose a system-call ABI through software interrupts.

An interrupt is not the same as a normal function call. The CPU saves return state, changes control flow through a descriptor, and expects the handler to return with `iretq` rather than `ret`.

## What the IDT stores

The Interrupt Descriptor Table (IDT) is an array of 256 gates on x86-64. Each gate is 16 bytes and contains the handler address split into pieces, a code-segment selector, an IST field, and gate attributes. The IDT register stores the table's byte limit and base address.

The current `os/interrupts.S` builds a 4096-byte table (256 × 16 bytes). It initially points every vector at a default stub, then installs specific stubs for vectors 0–47. The IDT is loaded with `lidt` only after the table has been filled.

Important values in this implementation:

- `0x08`: the 64-bit kernel code-segment selector from the GDT established by `os/boot.S`.
- `0x8e`: a present, ring-0 interrupt gate.
- `0..31`: CPU exception vectors.
- `32..47`: legacy PIC IRQ vectors after remapping.
- `255`: the diagnostic marker passed by the default stub for vectors without a dedicated entry.

The default stub's marker is not the original unexpected vector number. It is a deliberate fallback diagnostic; do not mistake it for a complete interrupt tracing facility.

## Why the PIC is remapped

The original 8259 PIC mapping overlaps the CPU's exception-vector range. VNTOS remaps the master PIC to start at vector 32 and the slave PIC at vector 40. This leaves vectors 0–31 for CPU exceptions and gives hardware IRQs a distinct range.

Only IRQ0 is unmasked in this iteration. The keyboard and other device IRQs remain masked; the PS/2 keyboard and mouse still use polling in `os/ps2.vnt`. Do not claim that keyboard input is interrupt-driven just because the kernel now has an IDT.

## Follow the source

Open `os/interrupts.S` and locate:

1. `idt_table` and `idt_pointer`;
2. `vntos_interrupts_init`;
3. `.Lfill_idt`, which initializes all 256 gates;
4. `.Linstall_stubs`, which replaces the first 48 gates;
5. `lidt`, which activates the table.

Then open `os/interrupts.vnt`. That file contains the policy function called after the assembly stub saves the CPU registers.

## What this implementation does not provide

It does not configure a local APIC, support multiple CPUs, switch privilege levels, provide a user-mode interrupt stack, implement a scheduler, or recover from page faults. A present IDT is necessary infrastructure, not a complete interrupt subsystem.

## Checkpoint

Explain why the IDT needs the address of each handler and why a normal function return is not a valid way to return from an interrupt.

## Exercise

Calculate the IDT storage size for 256 descriptors of 16 bytes each. Then find the code that writes the descriptor's low, middle, and high address fields.
