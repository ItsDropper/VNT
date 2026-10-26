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

## 6. Follow the kernel boundary in this repository

The word *operating system* can refer to the entire platform, including applications and system services. VNTOS in this course is primarily a kernel experiment. Its job is to establish a controlled execution environment and communicate with hardware. It does not yet provide the process isolation, filesystem, or application environment expected from a desktop operating system.

Look at the real boundaries:

- `os/boot.S` establishes the processor state needed to enter compiled kernel code.
- `os/kernel.vnt` owns kernel entry and shell command policy.
- `os/serial.vnt` implements serial device access.
- `os/console.vnt` combines VGA and serial output.
- `os/ps2.vnt` reads keyboard and mouse data using polling.
- `os/interrupts.S` handles architecture-specific interrupt entry and return.
- `os/interrupts.vnt` decides what a delivered event means to the kernel.

This is a useful architecture because device mechanics, policy, and CPU entry/exit are not all mixed into one file.

## 7. Driver versus policy

A driver knows how to communicate with a device. Policy decides what the system should do with the information. The PS/2 module knows how to read a scan code from controller ports; the shell decides what a completed line means. The serial driver knows when the UART can accept a byte; the console decides where a message should be sent.

This distinction becomes important when hardware changes. You can replace one output backend without rewriting command parsing if the shell depends on a stable console interface rather than raw device ports.

## 8. Why kernel code needs stricter rules

Kernel code runs without the safety net of a normal process. An invalid pointer can corrupt kernel state. A blocking loop can prevent the shell from responding. A malformed interrupt return frame can crash the machine. There is usually no independent process manager to kill the broken component and keep the rest of the system alive.

For every hardware operation, identify the preconditions, the possible failure, and the recovery policy. For example, a UART transmit function must decide what happens if the transmitter never becomes ready. An infinite wait may be acceptable for a tiny lab only if the limitation is explicit; it is not a robust general driver.

## 9. Trace a shell command as a miniature OS service

Run VNTOS in QEMU and enter `help`, then `about`, then `irq`. Find the dispatch branches in `os/kernel.vnt`. The shell reads characters, forms a command string, compares it with supported names, and invokes a function or prints a response. The `irq` command reads a timer counter; it does not itself deliver the interrupt.

This example shows how even a tiny shell separates input acquisition, parsing, policy, and device output. A larger system would add users, processes, files, permissions, and system calls, but those features need their own state and invariants rather than being bolted onto command strings.

## Lab deliverable

Draw a boundary diagram for one command. Mark which module reads the input, which function decides what the command means, which subsystem performs the operation, and which code displays the result. Label any layer that does not yet exist, such as a userspace process or syscall boundary.
