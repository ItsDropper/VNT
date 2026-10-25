# VNTOS: a tiny operating system built with VNT

VNTOS is an experimental x86-64 learning kernel. The Multiboot2 entry,
32-bit-to-long-mode transition, initial page tables, and bootstrap stack are
in os/boot.S. The kernel console, serial shell, and polled PS/2 input drivers
are written in VNT and compiled by the freestanding backend.

## Build

On Debian/Ubuntu/WSL, install the build and ISO tools:

    sudo apt-get update
    sudo apt-get install build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
    bash os/build.sh

The script builds the VNT compiler, compiles os/kernel.vnt, assembles the
small CPU bootstrap, and links a freestanding ELF and bootable ISO. The CI QEMU test also snapshots the VGA text buffer and verifies that the VNTOS banner is visible and stale SeaBIOS boot text is gone. The VGA
cursor references shared `vntos_state` storage, and PS/2 status polling uses
a bounded retry counter whose stack slot is not overwritten by inline asm.

## Run

Boot with a visible QEMU display and serial console:

    qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio

Use the QEMU window to see the VGA text console and type with the emulated
PS/2 keyboard. -display none -serial stdio remains useful for serial-only
debugging, but naturally hides VGA output.

Commands: help, about, clear, echo TEXT, mem, mouse, reboot, and halt.
COM1 serial output remains enabled alongside VGA output.

## Hardware implementation

- **VGA text console:** clears the 80×25 screen at kernel startup (removing
  leftover SeaBIOS text), writes character/attribute cells to physical address
  0xB8000, updates the hardware text cursor through CRTC ports, handles
  carriage return, newline, backspace, and scrolls the text buffer. `clear`
  blanks the screen, and `mem` prints hexadecimal addresses on VGA and serial.
- **PS/2 keyboard:** polls the 8042 status/data ports, decodes a basic Set 1
  scan-code subset, and supports letters, digits, space, Enter, Backspace,
  Tab, Shift, and Caps Lock. It is a polling driver, not an interrupt-driven
  input stack; extended keys and punctuation are not yet complete.
- **PS/2 mouse:** enables auxiliary-port streaming, assembles three-byte
  packets, tracks clamped text-cell coordinates and three button bits. mouse
  reports the current state. This is not a graphical pointer renderer.
- **Serial:** COM1 remains active for diagnostics and shell input. Input polling
  allows the PS/2 keyboard to work without blocking on serial reads.
- **Memory map:** the bootstrap identity-maps the first 1 GiB using 2 MiB
  pages, including the legacy VGA text buffer at 0xB8000.

## Limitations

This is a BIOS/GRUB-style Multiboot2 boot path, not a general UEFI GOP
framebuffer implementation. VGA text mode is legacy hardware and may not be
available in UEFI-only graphics environments. PS/2 support depends on the
emulator or machine exposing a compatible 8042 controller; USB-only input
devices may not work without firmware or emulator PS/2 emulation. Keyboard
input is polled, not interrupt-driven. There is no scheduler, userspace,
filesystem, heap allocator, or general driver framework yet.
