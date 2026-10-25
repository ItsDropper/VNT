# VNTOS: a tiny operating system built with VNT

VNTOS is an experimental x86-64 learning kernel whose main entry point is
written in VNT. A small assembly bootstrap handles the Multiboot2 entry,
32-to-64-bit CPU transition, page tables, COM1 serial output, and the command
shell.

For a beginner-friendly walkthrough, see
[Build and try VNTOS](../docs/tutorials/vntos-beginners.md).

## Build

On Debian/Ubuntu/WSL, install the build and ISO tools:

    sudo apt-get update
    sudo apt-get install build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
    bash os/build.sh

The build produces `build/kernel.elf` and `build/vntos.iso`. The script builds
the VNT compiler from source, compiles `os/kernel.vnt` with the freestanding
backend, and links the kernel without a hosted C runtime.

## Run

Start the serial shell in QEMU:

    qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio

Commands: `help`, `about`, `clear`, `echo TEXT`, `mem`, `reboot`, and
`halt`. The `mem` command reports linker-defined kernel boundaries and the
configured 1 GiB identity-map range; it does not detect installed RAM.

## Architecture and limitations

- GRUB Multiboot2 header and 32-bit bootstrap.
- First 1 GiB identity-mapped using 2 MiB pages before entering x86-64 long mode.
- VNT-generated `_start` and `kernel_main`.
- COM1 serial command shell with basic line editing.
- No filesystem, heap, userspace, scheduler, USB keyboard, networking stack, or
  general device-driver framework yet.
