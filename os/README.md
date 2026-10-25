# VNTOS: a tiny operating system built with VNT

VNTOS is a deliberately small x86-64 bare-metal learning OS. Its kernel entry is
written in VNT (kernel.vnt); a small assembly support file handles the CPU
mode transition, VGA text output, serial diagnostics, PS/2 keyboard polling,
and shell dispatch.

## Build

On Debian/Ubuntu/WSL, install the build and ISO tools:

    sudo apt-get update
    sudo apt-get install build-essential binutils grub-pc-bin grub-common xorriso qemu-system-x86
    bash os/build.sh

The build produces build/kernel.elf and, when GRUB and xorriso are installed,
build/vntos.iso. It compiles VNT from source and uses the VNT freestanding
backend; no hosted C runtime or libc is linked into the kernel.

## Run in QEMU

    qemu-system-x86_64 -cdrom build/vntos.iso -m 128M

Use the keyboard in the VGA console. Supported commands:

- help — list commands
- about — show kernel information
- clear — clear the text display
- reboot — request a keyboard-controller reset

The kernel also writes VNTOS_BOOT_OK to COM1 for automated boot checks.
The initial shell uses legacy VGA text memory and PS/2 scancodes; it is a QEMU
learning target, not a modern UEFI/GOP or USB-keyboard desktop OS.

## Architecture

- Multiboot2 header and 32-bit entry bootstrap.
- Identity map of the first GiB with 2 MiB pages, then transition to x86-64
  long mode.
- VNT-generated _start and kernel_main.
- No heap, filesystem, userspace, scheduler, or device-driver framework yet.
