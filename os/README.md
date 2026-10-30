# VNTOS: a tiny operating system built with VNT

VNTOS is an experimental x86-64 learning kernel. Its boot path and hardware
interrupt entry are deliberately split from the VNT policy code.

## Source layout

- `os/arch/x86_64/boot.S`: Multiboot2 header, 32-bit bootstrap, initial page
  tables, transition to long mode, and bootstrap stack.
- `os/arch/x86_64/interrupts.S`: IDT construction, minimal entry/exit stubs,
  legacy PIC remapping, and PIT setup.
- `os/kernel.vnt`: kernel entry and ordered subsystem initialization.
- `os/kernel/core.vnt`: early shared-state access, kernel bounds, and CPU halt/reboot primitives.
- `os/kernel/shell.vnt`: command parsing, line editing, and shell policy.
- `os/kernel/sync.vnt`: initial x86-64 spinlock primitives and an uncontended contract self-test.
- `os/serial.vnt`: COM1 UART setup and polling I/O.
- `os/console.vnt`: VGA text output and the combined VGA/serial console.
- `os/ps2.vnt`: polled PS/2 keyboard and mouse routines.
- `os/pic.vnt`: VNT-side legacy PIC IRQ mask controls and read-only live mask diagnostics; IRQ entry and EOI remain in assembly.
- `os/kernel/device_irq.vnt`: bounded accounting for legacy IRQ vectors 32–47; this is not yet a handler registry.
- `os/mm/phys.vnt`: Multiboot2-backed 4-KiB physical-frame allocator with bounded next-fit and contiguous-run APIs for the first 1-GiB mapped window.
- `os/mm/virt.vnt`: early virtual translation/mapping policy and a reversible mapping self-test.
- `os/interrupts.vnt`: VNT-side interrupt dispatch and fatal exception policy.
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
assembles the architecture bootstrap, interrupt entry, and AP trampoline separately, and links a freestanding ELF. Physical and early virtual-memory policy are currently implemented in VNT using low-level memory/CPU builtins. If GRUB and ISO tools are available, it creates
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

Commands: `help`, `about`, `clear`, `echo TEXT`, `history`, `uptime`, `acpitest`, `dectest`, `synctest`, `atomictest`, `mem`, `memtest`, `vmtest`,
`irq`, `mouse`, `smp`, `reboot`, and `halt`. The `help` output groups these commands into general, diagnostic, and power categories; the QEMU workflow checks those lines in the serial transcript. The PS/2 terminal supports Up-arrow history recall plus Left/Right, Home/End, Delete, insertion in the middle of a line, and Backspace. PS/2 navigation uses polled Set 1 scan codes. Serial input remains byte-oriented and does not decode incoming ANSI navigation sequences; cursor movement is sent to serial output using ANSI CSI-left. The `uptime` command reports decimal timer ticks and approximate seconds; it is not a wall clock. `uptime`, page-count output from `mem`, and IRQ counters from `irq` use decimal for readable quantities; addresses and register-like diagnostics remain hexadecimal. The uptime estimate is based on the configured 100 Hz timer. The `mem` command reports the kernel bounds and usable/free 4-KiB frame counts. `memtest` checks allocation alignment, next-fit cursor advancement, freeing, double-free rejection, and refusal to free the kernel start frame. `vmtest` temporarily remaps virtual address 256 MiB to an allocated frame, verifies translation and a write/read sentinel, then restores the identity mapping.
The timer and legacy IRQ delivery counters use aligned shared 64-bit slots and locked atomic increments; reads are snapshots and may change immediately after display. This avoids lost increments when multiple CPUs update a counter, but does not make unrelated kernel state, device handlers, or the physical allocator SMP-safe. The `acpitest` command cross-checks all sixteen legacy IRQ resolutions against the parsed MADT ISO entries, verifies invalid-input handling, and reports SKIP when no MADT is available. It rejects duplicate source overrides as ambiguous. The `irq` command reports the PIT tick count and legacy IRQ0/IRQ1 delivery counters, resolves each ISA IRQ to its effective GSI using ACPI MADT interrupt-source overrides (falling back to the conventional IRQ-to-GSI identity mapping when no override exists), and prints the raw override flags plus decoded polarity (active-high/active-low) and trigger mode (edge/level). ACPI conforming values use ISA defaults (active-high and edge-triggered). Reserved flag encodings and duplicate source entries return explicit errors instead of being silently interpreted. These values are diagnostic only: they do not configure an I/O APIC route or enable a device interrupt. The command then reads the live PIC mask bits for IRQ0 and IRQ1. It also reports Local APIC enablement, whether the supported I/O APIC masking setup completed, and the number of redirection entries masked by that setup. A zero route count alone does not mean there are no I/O APIC entries: check the separate readiness field. The counts show interrupt vectors dispatched, not whether a physical device is healthy. Keyboard IRQ1 remains masked because input is polled. The `atomictest` command arms a dedicated counter, sends an IPI to every online AP, and has the BSP and APs each perform 1,000 locked increments before checking the exact total. It requires at least two online CPUs and all discovered CPUs online; otherwise it reports SKIP. The test slots 308–312 are separate from live timer/IRQ counters. APs rendezvous at a ready/release barrier before their increment batches begin, increasing the likelihood of overlapping updates. It is a bounded QEMU-oriented smoke test, not a general proof of memory-ordering correctness. The `synctest` command checks uncontended spinlock acquisition, failed try-acquire, release, and double-release handling, then runs a bounded SMP contention test: the BSP and every online AP each perform 1,000 protected increments under the same spinlock after an IPI rendezvous. The four-CPU QEMU workflow requires the exact total of 4,000. This validates the current lock path under this test workload only; the physical allocator is not yet protected by the lock, and concurrent allocation still requires duplicate-page and free-count accounting tests.
Only IRQ0 is expected to advance with the current mask configuration; IRQ1
remains masked because keyboard input is polled.

## Interrupt architecture

The IDT contains 256 gates. The architecture-specific initializer fills the
table, installs dedicated stubs for CPU exception vectors 0–31 and legacy PIC
vectors 32–47, loads the IDT, remaps the PIC, programs PIT channel 0, unmasks
only IRQ0, and then enables maskable interrupts. The common assembly stub saves
the general-purpose registers, normalizes the vector/error-code stack shape,
calls `interrupt_dispatch` in VNT, acknowledges hardware IRQs, restores the
registers, and returns with `iretq`.

The VNT dispatcher routes vectors 32–47 through `device_irq_record()` in `os/kernel/device_irq.vnt`. IRQ0 also calls `kernel_timer_tick()` in `os/kernel/core.vnt`; the shell reads the named tick API and per-IRQ counters. `os/pic.vnt` exposes legacy PIC mask controls, and kernel startup explicitly keeps IRQ1 masked. CPU exceptions are reported and halted rather than incorrectly resumed. Page faults additionally print CR2 and the common present/write/user/reserved-bit/instruction-fetch flags; this is diagnostic only and does not recover from faults. Keyboard and mouse input are **still polled**. Do not unmask a device interrupt until its handler and data-ownership model are implemented.

## Current architecture boundary

`arch/x86_64/` owns the boot entry and the IDT/interrupt entry/EOI/PIT setup.
VNT-side policy is split between `interrupts.vnt`, `kernel/device_irq.vnt`,
and `pic.vnt`; the latter changes PIC masks but does not install handlers.
The existing shell and device routines retain their behavior. Kernel entry now owns initialization order, while core primitives and shell policy live in `os/kernel/core.vnt` and `os/kernel/shell.vnt`. This
is a source/build boundary, not yet a fully abstract architecture interface:
the VNT compiler currently flattens imported modules into one compilation
unit, and some low-level VNT functions use compiler-specific stack offsets.
The shared scalar state is still defined by the bootstrap assembly and read
or written through inline assembly in VNT. These are explicit follow-up tasks,
not evidence of independent modules or memory-safe ownership.

## Hardware implementation and limits

- **VGA text console:** 80×25 text cells at physical address `0xB8000`, CRTC
  cursor updates, newline/backspace, and scrolling.
- **Serial:** COM1 remains available for diagnostics and shell input.
- **PS/2 keyboard:** polls the 8042 status/data ports and translates a basic
  Set 1 scan-code subset. Extended keys and punctuation are incomplete.
- **PS/2 mouse:** polls and assembles three-byte packets, tracking clamped
  text-cell coordinates and button bits. There is no graphical pointer.
- **Memory map:** the bootstrap identity-maps the first 1 GiB using 2 MiB
  pages, including the legacy VGA text buffer. The physical allocator parses
  Multiboot2 type-1 regions inside that window and tracks usable, allocated,
  and reserved frames with bitmaps. Available regions are processed first, then all non-available regions clear any overlapping frames from the usable bitmap. This makes reserved entries take precedence even if firmware entries overlap. Allocation searches from a cursor and wraps once; this reduces repeated scans of already-used low frames but does not make the allocator SMP-safe.
- **Timer:** PIT-based, approximately 100 Hz. It is not a calibrated wall
  clock and does not schedule tasks.

This is a BIOS/GRUB-style Multiboot2 boot path, not a general UEFI GOP
framebuffer implementation. VGA text mode may be unavailable in UEFI-only
graphics environments. PS/2 input depends on the emulator or machine exposing
a compatible controller.

VNTOS still has no scheduler, userspace, filesystem, general heap allocator,
virtual-memory manager, or general device-driver framework. The physical
allocator is limited to the current 1-GiB identity-mapped window and is not
safe for concurrent allocation from multiple CPUs. The VNT
freestanding backend is experimental and some existing low-level routines
still rely on compiler-specific stack offsets. Treat this as a learning kernel,
not production-ready OS code.
