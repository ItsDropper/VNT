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

## Read one 16-byte gate as a data structure

An x86-64 interrupt gate contains these fields:

| Byte offsets | Size | Meaning |
| --- | ---: | --- |
| 0–1 | 2 bytes | handler address bits 0–15 |
| 2–3 | 2 bytes | kernel code selector |
| 4 | 1 byte | IST index and reserved bits |
| 5 | 1 byte | present/type/privilege attributes |
| 6–7 | 2 bytes | handler address bits 16–31 |
| 8–11 | 4 bytes | handler address bits 32–63 |
| 12–15 | 4 bytes | reserved, written as zero |

The address is split because the descriptor layout is fixed by the processor architecture. The initializer writes the same handler address into the low, middle, and high pieces. When it installs a dedicated stub, it overwrites those pieces with that stub's address.

The IDT limit is measured in bytes minus one. For 256 entries:

`256 × 16 = 4096` bytes, so the limit is `4095`.

## Why install the IDT before enabling IRQs?

An interrupt can arrive as soon as its line is unmasked and the CPU's interrupt flag is enabled. If the IDT is not ready, the processor may follow an invalid descriptor and fault again while trying to report the original fault. That can end in a triple fault and an emulator reset.

The initializer therefore fills the table, installs the known stubs, executes `lidt`, remaps the PIC, programs the PIT, masks all but IRQ0, and only then executes `sti`. Do not move `sti` to the beginning as a cosmetic cleanup.

## Manual review exercise

In `os/interrupts.S`, inspect each write to offsets 0, 2, 4, 5, 6, 8, and 12 from the gate pointer. For every write, name the field from the table above. Then inspect the code that constructs the IDTR pointer: it stores a 16-bit limit followed by a 64-bit base.

The initial page tables map the low 1 GiB, so the IDT and handlers at the kernel's current low physical/virtual addresses are reachable under the current identity-map design. If the kernel moves to a higher-half layout later, the address assumptions and mapping strategy must be revisited.

## Failure analysis

A missing gate can fault when the event occurs. A wrong selector can cause a general-protection fault. A malformed stack frame can make the return path interpret the error code as RIP. A wrong PIC vector offset can send IRQ0 into an exception handler. These failures look similar from the outside—a frozen VM or reboot—so keep the table layout, stub frame, and PIC configuration as separate review steps.

## A final architecture sanity check

The first 32 vectors belong to CPU-defined exceptions. They are not interchangeable with device IRQ numbers. The legacy PIC mapping used here places IRQ0 at vector 32, IRQ1 at 33, and so on. A modern APIC-based machine has more interrupt-routing machinery; this course intentionally starts with the legacy PIC because the current QEMU kernel uses it.

The IDT table is allocated in the kernel's BSS, not on the stack. The descriptor base points at that static storage, which remains alive after `interrupts_init` returns. If the table were a local variable, the CPU could later follow a pointer into reused stack memory.

When reviewing any future IDT change, answer these questions before running it:

- Is the table base correctly aligned and mapped?
- Is the limit the final byte index, not the number of entries?
- Does every installed gate point to executable kernel code?
- Is the selector valid in the active GDT?
- Are exception error-code conventions correct?
- Are hardware IRQs mapped away from the CPU exception range?
- Is the table loaded before the CPU is allowed to receive maskable IRQs?

These are invariants, not style preferences.
