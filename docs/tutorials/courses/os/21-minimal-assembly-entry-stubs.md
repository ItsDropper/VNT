# Lesson 21 — The Small Assembly Boundary

**Goal:** read the interrupt entry stubs and understand exactly why a small amount of assembly is required.

## Why not write the entire handler in VNT?

The compiler must enter a function with a valid ABI. An interrupt can arrive between arbitrary instructions, not just at a point where a normal function call would be safe. The CPU's stack frame also differs from a normal call frame. Before VNT code can run, the entry path must normalize that frame and preserve the interrupted program's registers.

VNT's current inline-assembly form has no input/output operands, register constraints, or clobber list. Using it to write a complete interrupt entry routine would hide too much information from the compiler. A short standalone assembly file is easier to audit than pretending the current inline assembly feature is a complete interrupt ABI.

## One stub, one normalized frame

In `os/interrupts.S`, a no-error-code entry uses this macro:

```asm
.macro ISR_NOERR number
.global isr\number
isr\number:
    pushq $0
    pushq $\number
    jmp interrupt_common
.endm
```

The first push adds a synthetic error code of zero. The second adds the vector number. This makes the top of the stack consistent for exceptions that do not receive an error code from the CPU.

Some exceptions already push an error code. Their `ISR_ERROR` macro pushes only the vector; adding another synthetic error code would shift the frame and make the dispatcher read the wrong values.

## What the common entry must preserve

The common entry saves all 15 general-purpose registers other than RSP itself. It then reads the vector and error code from the normalized frame and passes them as the first two arguments to `interrupt_dispatch`, following the x86-64 System V calling convention used by this build.

Before calling VNT, it aligns the stack to a 16-byte boundary. The original stack pointer is held in R12, which the ABI requires the called function to preserve. After VNT returns, the assembly acknowledges the PIC for hardware IRQ vectors, restores the saved registers, removes the two normalized frame fields, and executes `iretq`.

The interrupt gate clears IF on entry, so the timer interrupt cannot recursively interrupt the dispatcher while it updates the tick count. This implementation intentionally does not enable nested interrupts.

## Why the error-code list matters

The macro list marks the exceptions that push CPU error codes in this target. The exact list is architecture- and CPU-feature-sensitive. If you add another vector or target a different processor mode, check the processor manual before assuming whether an error code exists.

A wrong stack shape can turn one exception into a second fault while trying to report the first one. That is why this code deserves more scrutiny than ordinary shell logic.

## Assembly should stop at the boundary

The assembly file does the jobs that require exact CPU control: creating/loading the IDT, remapping the PIC, programming PIT channel 0, saving registers, and executing `iretq`. The VNT module decides whether a vector is a timer tick or a fatal exception.

That is the design goal—not zero assembly at any cost. The amount of assembly is minimized by giving it a narrow contract.

## Checkpoint

Draw the stack from the vector number down through the CPU-saved return state. Explain the difference between `ret` and `iretq`.

## Exercise

Inspect the push and pop order in `interrupt_common`. Verify that each saved register is restored in reverse order and that the vector/error pair is removed before `iretq`. Do not reorder the pushes as a cleanup.
