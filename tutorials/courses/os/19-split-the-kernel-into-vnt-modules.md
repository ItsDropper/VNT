# Lesson 19 — Split the Kernel into VNT Modules

**Goal:** understand the difference between VNT source modules and the
architecture-specific assembly boundary in the working VNTOS build.

## Current ownership

| File | Owns |
| --- | --- |
| `os/kernel.vnt` | kernel entry and ordered subsystem initialization |
| `os/kernel/core.vnt` | shared state access, kernel bounds, CPU halt/reboot |
| `os/kernel/shell.vnt` | shell input loop, string helpers, command policy |
| `os/serial.vnt` | COM1 initialization and byte/string I/O |
| `os/console.vnt` | VGA text output, cursor, scrolling, combined console, hexadecimal output |
| `os/ps2.vnt` | polled 8042 keyboard/mouse input and scan-code translation |
| `os/interrupts.vnt` | VNT-side interrupt dispatch and fatal exception policy |
| `os/kernel/device_irq.vnt` | Bounds-checked accounting for legacy IRQ lines 0–15 |
| `os/pic.vnt` | Legacy 8259 PIC mask controls; not a handler registry |
| `os/arch/x86_64/boot.S` | Multiboot2 bootstrap and transition into 64-bit mode |
| `os/arch/x86_64/interrupts.S` | IDT setup, CPU entry/exit, PIC remapping and PIT setup |

The two assembly files are now physically located under the architecture
boundary and the build script assembles them from those paths. The shell still
calls the same console and input routines. This change is intentionally a
source/build refactor: the assembly instructions and exported names are kept
stable to avoid changing the working boot and interrupt ABI.

## Understand VNT imports

At the top of `os/kernel.vnt`, the imports now include the core and shell modules:

```vnt
import "serial.vnt"
import "console.vnt"
import "ps2.vnt"
import "interrupts.vnt"
import "kernel/core.vnt"
import "kernel/shell.vnt"
```

Each path is relative to the importing file. Since the root file lives in
`os/`, these names still resolve to `os/serial.vnt`, `os/console.vnt`,
`os/ps2.vnt`, and `os/interrupts.vnt`.

The module loader combines imported declarations into the same freestanding
compilation unit. Imports do not create separate ELF files, processes, address
spaces, or independently loadable modules. Assembly remains a separate object
because the CPU interrupt frame and `iretq` return sequence require a precise
ABI boundary.

## Trace a shell call

1. In `os/kernel.vnt`, find `kernel_main()` and the final call to `shell_run()`.
2. In `os/kernel/shell.vnt`, find a call to `console_puts("VNTOS> ")`.
3. Open `os/console.vnt` and find `console_puts`.
4. Follow it to `vga_puts` and `serial_puts`.
5. Open `os/serial.vnt` and find `serial_puts`, then `serial_putc`.

Shared state and CPU halt/reboot primitives are isolated in `os/kernel/core.vnt`. This is a source boundary only: the current assembly-backed state access still depends on compiler-specific stack offsets.

Now trace the timer path: `os/arch/x86_64/interrupts.S` enters the CPU-defined
frame, `os/interrupts.vnt` classifies vectors 32–47, and `os/kernel/device_irq.vnt`
records the IRQ count. Vector 32 additionally increments the named timer count;
the shell reads both through `irq`. The counter is not a scheduler. `os/pic.vnt`
can mask or unmask a legacy PIC line, but it does not create a safe device handler.

## Build and verify

From the repository root:

```sh
bash os/build.sh
```

The script builds the VNT compiler, follows the imports from
`os/kernel.vnt`, emits freestanding assembly, assembles the two files under
`os/arch/x86_64/`, and links the kernel. Inspect the GitHub Actions
`VNTOS Boot and Serial Shell` run for QEMU evidence; a successful link alone
does not prove the keyboard or timer path works.

## Important limitations

The architecture boundary is incomplete by design. `boot.S` still owns the
shared scalar storage `vntos_state`; VNT functions read/write it using
compiler-specific inline assembly. Several functions assume particular stack
offsets in generated code. These assumptions are not a stable typed ABI, and
the current module loader flattens VNT imports into one compilation unit.

Do not change the shared-state helpers as part of an unrelated file move.
First document and test the compiler ABI, then replace stack-offset tricks with
explicit supported mechanisms. Keep keyboard IRQ1 masked until an interrupt-safe
input queue and clear ownership model exist.

## Exercise

Pick a function with one responsibility and a stable call name. Identify which
module owns it and explain why. Do not move it until the build and relevant
runtime test can be run. After a refactor, inspect the diff, run the build, and
verify the behavior it owns rather than relying on the prompt alone.
