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