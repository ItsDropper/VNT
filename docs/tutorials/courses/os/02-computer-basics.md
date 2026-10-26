# Lesson 02 — The Machine Underneath the OS

**Goal:** learn the minimum computer architecture vocabulary needed to read the kernel.

## 1. CPU, RAM, and devices

The **CPU** executes instructions. **RAM** holds instructions and data currently in use. Devices provide capabilities such as storage, timers, input, or output.

A kernel is software, but it must obey the CPU's rules. Before normal compiled code runs, the CPU must be in a compatible execution mode and have a valid stack. The kernel must also arrange memory so the addresses used by its code can be accessed.

## 2. Bits and bytes

A bit is either 0 or 1. A byte is 8 bits. A 32-bit register holds 32 bits; a 64-bit register holds 64 bits. A 64-bit CPU mode has 64-bit general-purpose registers and a 64-bit instruction environment, but this does **not** mean every value or every device is 64-bit.

The kernel uses integer values for counters and characters, and addresses must be wide enough to represent the memory locations being used. Pay attention to a type's actual width instead of assuming every integer is a pointer.

## 3. Registers and the instruction pointer

Registers are small storage locations inside the CPU. On x86-64, examples include:
- `RAX`, often used for arithmetic and return values;
- `RSP`, the stack pointer;
- `RBP`, commonly used as a frame pointer by some generated code;
- `RIP`, the instruction pointer identifying the next instruction.

The exact use of a register depends on the calling convention and compiler output. Do not assume a function argument always lives in a specific stack slot; inspect the generated assembly when debugging.

## 4. The stack

A function needs temporary storage for values, return information, and sometimes local variables. The stack is a region of memory managed using a stack pointer. A function call and return convention determines how arguments, saved registers, and return values are handled.

The bootstrap must establish a valid stack before calling compiled code. VNTOS currently uses a stack reserved in `os/boot.S`. Later, `kernel_main` also reserves space for its command buffer. This is an implementation detail to inspect—not a general pattern to copy into every program.

## 5. Addresses are not automatically safe

A number that looks like an address is not proof that memory exists or is safe to access. The CPU's paging configuration determines how virtual addresses are translated. Hardware devices may also expose registers through ports or memory-mapped regions.

VNTOS initially maps the first 1 GiB of address space using large pages. This is only an early mapping strategy. It does not discover installed RAM, reserve all hardware regions correctly, or implement a memory allocator.

## 6. What is a privilege level?

Modern x86 CPUs support privilege levels. Operating systems commonly run their kernel at ring 0 and ordinary applications at ring 3. Code running at a more restricted privilege level cannot freely execute every privileged instruction or access every resource.

The current VNTOS shell runs in kernel context. It is not an isolated user program. Do not describe it as ring 3.

## Checkpoint

Define CPU, RAM, register, stack, address, paging, and privilege level in your own words.

## Exercise

Open `os/boot.S` and find where `RSP` is initialized in long mode. Explain why the bootstrap must do that before calling `_start`.

## 7. Derive a stack frame instead of memorizing offsets

On x86-64, a normal function call conventionally uses a stack pointer named RSP and a frame pointer named RBP when the compiler chooses to keep one. A push of a 64-bit register moves RSP by eight bytes; a pop reverses that movement. If a function saves registers or allocates locals, the addresses of its locals depend on those exact operations.

That is why hard-coded offsets in inline assembly are brittle. If the compiler changes its prologue or a local variable is added, an expression such as `-16(%rbp)` may no longer refer to the intended value. VNTOS has some routines that rely on this compiler-specific layout. Treat them as known limitations, not as a general technique to copy into new code.

## 8. Memory-mapped devices and port I/O

The VGA text buffer is memory-mapped: software writes character and attribute bytes to a physical address that the hardware interprets as display cells. The legacy serial and PS/2 controllers use x86 I/O ports instead. These are different access mechanisms. A pointer store is not interchangeable with an `in` or `out` instruction.

The boot code identity-maps the first 1 GiB, which lets the kernel access low physical addresses using the same numeric virtual addresses for now. This is a bootstrap convenience, not a universal rule. Once a kernel changes its mappings, a physical address and a virtual address can refer to different locations.

## 9. Interrupts are not ordinary function calls

A normal function call happens because code deliberately calls another function. An interrupt transfers control because of a processor exception or a device event. The CPU constructs a return frame, and the entry stub must preserve state before calling ordinary compiled code. The handler returns with `iretq`, not a normal `ret`, because the CPU must restore the interrupted instruction pointer and flags.

The current timer handler increments a counter and returns to the interrupted code. It does not switch tasks. A timer interrupt by itself is not a scheduler.

## 10. Privilege and trust boundaries

x86 privilege levels determine which instructions and memory accesses are allowed. VNTOS currently operates in kernel mode; it does not have a user-mode process boundary. Therefore the shell is not an isolated program, and a bad command implementation can damage the entire kernel.

Before adding userspace, the kernel would need controlled transitions, syscall entry, memory protection, per-process address spaces, and a defined ABI. Do not describe a function in the shell as a system call simply because it performs a privileged operation.

## Lab: map one hardware access

Choose one access in `os/serial.vnt`, `os/console.vnt`, or `os/ps2.vnt`. Record the address or port, the width of the access, the status bit or memory layout it relies on, and the function that owns the operation. Explain what would break if the access were moved into the shell without a wrapper.

## 11. Understand why kernel debugging is different

In a hosted application, the operating system usually isolates a process from unrelated processes. In this kernel, most bugs occur in the one privileged execution environment. A bad write can damage the stack, the IDT, page tables, or shell state. A fault handler itself can fail if its stack or code mapping is invalid.

For this reason, begin with passive inspection and reversible QEMU experiments. Prefer reading ELF metadata, adding one serial marker, or examining the current source over intentionally triggering faults on real hardware. A test should have a known reset path and should not depend on a disk image containing personal data.

## 12. Convert an assumption into a check

Examples of assumptions worth checking:

- "This pointer is mapped" → identify the page-table range that maps it.
- "This function exists in the ELF" → inspect the symbol table.
- "The timer runs" → read the counter twice and compare values.
- "The shell reads keyboard input" → identify whether input comes from PS/2 or serial.
- "The compiler supports this syntax" → build a minimal example and run the relevant test.
- "The ISO boots" → run QEMU and record the visible boot marker.

A useful kernel developer turns assumptions into checks that another person can repeat.

## 13. A register is part of the interface

When an assembly stub calls compiled VNT code, the calling convention determines where arguments are placed and which registers the callee must preserve. An interrupt stub has an additional responsibility: it must preserve the state of the code it interrupted, even if that state was not created by an ordinary call. These are related but different contracts.

Before reviewing an assembly-to-VNT call, identify the argument registers, saved registers, stack alignment, return value, and symbol name. If one is undocumented, treat it as an assumption to verify against the emitted assembly and ELF symbols.
