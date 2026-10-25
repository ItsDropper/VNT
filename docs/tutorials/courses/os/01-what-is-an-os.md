# Lesson 01 — What an Operating System Does

**Goal:** understand the job of an operating system and what VNTOS does—and does not—do today.

## 1. Programs need an environment

An ordinary application usually expects another program to have prepared its environment. On Windows, an application can ask the operating system to open a file, allocate memory, create a process, or send data through a network connection.

A kernel starts much closer to the hardware. It cannot assume that Windows APIs, a C standard library, a filesystem, or a process manager exist. If the kernel needs output, it must implement or initialize a path to hardware that can display or transmit bytes.

## 2. The kernel is not the whole operating system

A complete OS commonly includes a kernel, device drivers, system services, libraries, utilities, and user applications. These parts cooperate, but they are not all the kernel.

The current VNTOS learning kernel has a narrow set of responsibilities:
- receive control through its boot path;
- establish a basic CPU execution environment;
- use a serial device for text input and output;
- run a simple command loop.

It does **not** currently provide a complete filesystem, multitasking, user-mode applications, a network stack, or a general-purpose physical memory allocator. A shell prompt alone does not make those features exist.

## 3. Resource management

A kernel eventually needs rules for shared resources:

- **CPU time:** which task runs and when;
- **memory:** which physical and virtual addresses are safe to use;
- **devices:** how software communicates with hardware;
- **storage:** how data is organized and retrieved;
- **protection:** how one program is prevented from corrupting another.

These are separate subsystems. They should be designed and tested individually rather than all being added in one large change.

## 4. Why start with a serial shell?

A graphical desktop needs many layers: display hardware, graphics modes, input, memory management, fonts, windowing, and more. Serial output is much simpler. QEMU can connect the guest's serial port to your terminal, so a kernel can show progress even when no graphical display is initialized.

The VNTOS shell is therefore a useful first milestone. It proves that the CPU reached compiled kernel code and that basic input/output works. It does not prove that every CPU feature or memory address is configured correctly.

## 5. Follow one command

When you type `about` into the QEMU serial terminal:

1. QEMU passes input bytes to the guest's serial device.
2. `serial_getc()` waits for an available byte and reads it.
3. The shell stores characters in its command buffer.
4. On Enter, the shell compares the buffer with known command names.
5. The `about` branch prints a response through `serial_puts()`.

You will inspect these functions in Lesson 14.

## Checkpoint

Explain the difference between:
- a kernel and a complete operating system;
- a bootloader and a kernel;
- successful compilation and successful booting.

## Exercise

List three features that VNTOS does not yet implement. For each, name one reason it would need careful design. Do not claim a feature exists just because its name appears in a roadmap or documentation.