# VNTOS: a tiny operating system built with VNT

VNTOS is an experimental x86-64 learning kernel. The boot protocol, initial
CPU-mode transition, page tables, and bootstrap stack are implemented in
assembly in `os/boot.S`. The kernel entry and serial shell are compiled from
`os/kernel.vnt` by VNT's freestanding backend. Hardware I/O and CPU control
operations still use low-level inline assembly in the VNT source.

Start with the [VNT programming course index](../docs/tutorials/README.md) or
the focused [VNTOS kernel walkthrough](../docs/tutorials/05-vntos-kernel-walkthrough.md).

## Build

On Debian/Ubuntu/WSL, install the build and ISO tools:

    sudo apt-get update
    sudo apt-get install build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
    bash os/build.sh

The script builds the VNT compiler from source, compiles `os/kernel.vnt` with
the freestanding backend, assembles `os/boot.S`, and links the kernel without
a hosted C runtime. It produces `build/kernel.elf` and, when the GRUB/ISO
tools are available, `build/vntos.iso`.

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
- COM1 serial shell with basic line editing.
- No filesystem, heap, userspace, scheduler, USB keyboard, networking stack, or
  general device-driver framework yet.
- Assembly should remain limited to bootstrap and any future interrupt-entry
  stubs; shell behavior and ordinary kernel subsystems belong in VNT as the
  freestanding compiler gains the required capabilities.
