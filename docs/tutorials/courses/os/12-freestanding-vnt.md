# Lesson 12 — Hosted Programs Versus Freestanding Kernel Code

**Goal:** understand why compiling VNT for a kernel is different from compiling an ordinary VNT program.

## 1. A hosted program

A normal VNT executable is built to run under an existing operating system. It can link against VNT's native runtime and use services provided by Windows or a Unix-like system. That environment supplies process startup, memory management, file access, and many other facilities.

A kernel is the environment that normally provides those facilities to other programs. It cannot call its own host operating system to get them before the kernel exists.

## 2. Freestanding means fewer assumptions

The build script uses the compiler's freestanding mode to emit assembly for `os/kernel.vnt`, then assembles and links it without a hosted C runtime. The kernel needs a defined entry point and must avoid depending on hosted APIs that are unavailable at boot.

The freestanding backend currently supports a restricted subset of VNT. It does not imply that every standard-library module or hosted language feature works in the kernel. Check the current compiler documentation and tests before relying on a feature.

## 3. Entry and function flow

The linker enters at `boot32_entry`, then the assembly eventually calls the generated `_start`. The freestanding backend emits the startup glue that reaches `kernel_main`.

This is why `kernel_main` is not the ELF entry point. It is the main kernel function after the CPU bootstrap and generated entry code have run.

## 4. Inline assembly is a sharp tool

The current `kernel.vnt` uses inline assembly for:
- UART port I/O;
- CPU halt and reboot control;
- reading linker-defined symbol addresses;
- a few operations that rely on the compiler's current stack-frame layout.

These are implementation constraints, not ideal long-term interfaces. Hard-coded stack offsets are especially brittle: a compiler optimization or calling-convention change could move a local variable. Proper VNT intrinsics or small, well-defined compiler-supported primitives would be safer than depending on incidental stack layout.

Do not blindly copy the inline assembly into another function. Before modifying it, inspect the generated assembly and understand the exact calling convention.

## 5. What the kernel currently lacks

The current kernel is not linked with a hosted runtime, and it does not yet have all the infrastructure needed for dynamic allocation, filesystem access, user programs, or general device drivers. Adding a call to a hosted function does not magically implement that subsystem.

A good systems-language compiler gradually adds a tested freestanding subset, explicit hardware intrinsics, and ABI tests. Unsupported features should fail clearly instead of silently generating incorrect code.

## Checkpoint

Explain why `print()` from a hosted VNT program is not automatically the right output function for a kernel. Explain why `kernel_main` is not the ELF entry.

## Exercise

Run `bash os/build.sh`, then inspect the generated `build/kernel.s`. Find the generated `_start` symbol and compare it with the linker entry. Treat generated assembly as diagnostic output; do not hand-edit it as a lasting fix.