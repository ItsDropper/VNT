# VNTOS: a tiny operating system built with VNT

VNTOS is an experimental x86-64 learning kernel. Its boot path and hardware
interrupt entry are deliberately split from the VNT policy code.

## Source layout

- `os/boot.S`: Multiboot2 header, 32-bit bootstrap, initial page tables,
  transition to long mode, and bootstrap stack.
- `os/kernel.vnt`: kernel entry, shared state helpers, CPU control, and shell
  command policy. It imports the other VNT modules.
- `os/serial.vnt`: COM1 UART setup and polling I/O.
- `os/console.vnt`: VGA text output and the combined VGA/serial console.
- `os/ps2.vnt`: polled PS/2 keyboard and mouse routines.
- `os/interrupts.vnt`: VNT-side interrupt dispatch and fatal exception policy.
- `os/interrupts.S`: IDT construction, minimal entry/exit stubs, legacy PIC
  remapping, and PIT setup. Assembly is limited to the CPU/ABI boundary.
- `os/linker.ld`: ELF layout and linker symbols.
- `os/grub.cfg`: GRUB's Multiboot2 kernel configuration.
- `os/build.sh`: compiler build, VNT module compilation, assembly, linking,
  and optional ISO packaging.

## Build

On Debian/Ubuntu/WSL, install the build and ISO tools:

```sh
sudo apt-get update
sudo apt-get install build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
bash os/build.sh
```

The script follows imports from `os/kernel.vnt`, emits freestanding assembly,
assembles the boot and interrupt boundary files separately, and links a
freestanding ELF. If GRUB and ISO tools are available, it creates
`build/vntos.iso`. The CI QEMU test validates the Multiboot2 ELF, checks for
unresolved symbols, exercises the serial shell and keyboard path, and checks
that the VGA banner replaces stale SeaBIOS text.

## Run

Boot with a visible QEMU display and serial console:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Use the QEMU window to see the VGA text console and type with the emulated
PS/2 keyboard. For serial-only debugging, use:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Commands: `help`, `about`, `clear`, `echo TEXT`, `mem`, `irq`,
`mouse`, `reboot`, and `halt`. The `irq` command reports the count of
PIT timer interrupts delivered since initialization. Run it twice with a
delay between commands; the count should increase.

## Interrupt architecture

The IDT contains 256 gates. The assembly initializer fills the table, installs
dedicated stubs for CPU exception vectors 0–31 and legacy PIC vectors 32–47,
loads the IDT, remaps the PIC, programs PIT channel 0, unmasks only IRQ0, and
then enables maskable interrupts. The common stub saves the general-purpose
registers, normalizes the vector/error-code stack shape, calls
`interrupt_dispatch` in VNT, acknowledges hardware IRQs, restores the
registers, and returns with `iretq`.

The VNT dispatcher increments a shared tick counter for vector 32. CPU
exceptions are reported and halted rather than incorrectly resumed. The
keyboard and mouse are **still polled**: IRQ1 and the other PIC lines remain
masked. Do not unmask a device interrupt until its handler and data-ownership
model are implemented.

## Hardware implementation and limits

- **VGA text console:** 80×25 text cells at physical address `0xB8000`, CRTC
  cursor updates, newline/backspace, and scrolling.
- **Serial:** COM1 remains available for diagnostics and shell input.
- **PS/2 keyboard:** polls the 8042 status/data ports and translates a basic
  Set 1 scan-code subset. Extended keys and punctuation are incomplete.
- **PS/2 mouse:** polls and assembles three-byte packets, tracking clamped
  text-cell coordinates and button bits. There is no graphical pointer.
- **Memory map:** the bootstrap identity-maps the first 1 GiB using 2 MiB
  pages, including the legacy VGA text buffer.
- **Timer:** PIT-based, approximately 100 Hz. It is not a calibrated wall
  clock and does not schedule tasks.

This is a BIOS/GRUB-style Multiboot2 boot path, not a general UEFI GOP
framebuffer implementation. VGA text mode may be unavailable in UEFI-only
graphics environments. PS/2 input depends on the emulator or machine exposing
a compatible controller.

VNTOS still has no scheduler, userspace, filesystem, general heap allocator,
physical-page allocator, or general device-driver framework. The VNT
freestanding backend is experimental and some existing low-level routines
still rely on compiler-specific stack offsets. Treat this as a learning kernel,
not production-ready OS code.
