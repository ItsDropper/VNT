# Tutorial 5: Build the VNTOS Learning Kernel

This walkthrough explains the kernel in `os/` and how its VNT source becomes a bootable ELF. VNTOS is an experimental learning project. Build and run it in QEMU; do not write its image to a physical disk.

## 1. What is in `os/`?

- `os/arch/x86_64/boot.S`: Multiboot2 header and the low-level CPU bootstrap. It starts in 32-bit mode, sets up page tables, enables long mode, and transfers control to the kernel entry.
- `os/kernel.vnt`: VNT source for the kernel's high-level entry and shell logic supported by the current branch.
- `os/linker.ld`: Linker layout, including where the kernel is placed and the symbols used to describe its boundaries.
- `os/grub.cfg`: GRUB configuration for loading the kernel using Multiboot2.
- `os/build.sh`: Builds the VNT compiler, compiles the freestanding VNT source, assembles the bootstrap, links the ELF, and optionally packages a GRUB ISO.

The exact division of work can change as VNT gains more freestanding capabilities. Read these files on your checked-out branch rather than assuming a tutorial describes every future revision.

## 2. Understand the boot path

The intended path is:

1. GRUB loads the ELF kernel through Multiboot2.
2. The bootstrap in `boot.S` initializes the early CPU state and page tables.
3. The bootstrap switches to x86-64 long mode and establishes a stack.
4. Control reaches the generated VNT entry point.
5. That entry point calls the kernel function in `kernel.vnt`.

The assembly is needed at the earliest stage because the CPU must be placed into a known execution mode before ordinary compiled kernel code can run. The goal is to keep this file limited to bootstrap and any low-level interrupt entry code the kernel eventually requires—not to put the shell or normal kernel subsystems in assembly.

## 3. Build in a Linux environment

Install the required tools on Debian, Ubuntu, or WSL:

```bash
sudo apt-get update
sudo apt-get install --yes build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
```

From the repository root:

```bash
bash os/build.sh
```

The main outputs are:

- `build/kernel.elf`: the linked freestanding ELF64 kernel.
- `build/vntos.iso`: the bootable ISO, if GRUB and ISO packaging tools are available.

If compilation fails, read the first compiler or linker error; later errors may be consequences of the first one.

## 4. Boot the kernel in QEMU

```bash
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

The terminal is connected to the guest's serial port. Try the commands printed by the shell's `help` command. Use `Ctrl+C` in the terminal to stop the emulator.

## 5. Follow a command through the source

Open `os/kernel.vnt` and trace these responsibilities in order:

1. Serial initialization prepares the legacy COM1 UART.
2. Character input reads one byte from the serial device.
3. Character output sends text back to the terminal.
4. String helpers compare command text and print strings.
5. The main loop collects a command line, handles editing, dispatches recognized commands, and prints the prompt again.

On this early kernel, hardware port I/O and CPU control instructions require low-level operations. As VNT's compiler gains proper hardware intrinsics, interrupt support, and memory-mapped I/O APIs, those operations should live behind small VNT functions rather than spreading assembly through application logic.

## 6. Understand the memory report

The current `mem` command reports linker-defined kernel boundaries and the configured identity-mapped address range. It does not detect installed RAM, provide a physical-memory allocator, or prove that every mapped address is backed by usable memory.

The bootstrap maps the first 1 GiB using 2 MiB pages. This is an initial mapping to get the kernel running, not a complete virtual-memory subsystem.

## 7. Verify changes safely

After changing the kernel, rebuild and run it in QEMU. Also check the GitHub Actions workflow for the boot and serial-shell tests. A successful compilation alone does not prove that the kernel reaches its prompt or handles input correctly.

Useful follow-up work:

- Replace fragile inline assembly that accesses compiler-specific stack offsets with compiler-supported hardware intrinsics.
- Keep `boot.S` focused on bootstrap duties.
- Add interrupt descriptor-table setup and interrupt entry stubs only when implementing interrupts.
- Add tests for each new kernel subsystem before depending on it.

## Exercises

1. Find the instruction in `boot.S` that jumps into long mode.
2. Trace the entry function from the generated `_start` to `kernel_main`.
3. Add a harmless new shell command in VNT and test it in QEMU.
4. Write down which operations still need hardware-specific support and why.
