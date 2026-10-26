# Lesson 19 — Split the Kernel into VNT Modules

**Goal:** learn the actual module boundaries in VNTOS and understand how several VNT source files become one freestanding kernel.

## What changed

The kernel used to put device I/O, console drawing, PS/2 input, and shell policy in one large file. That made it hard to find a bug without scrolling through unrelated code. The current layout separates responsibilities:

| File | Owns |
| --- | --- |
| `os/kernel.vnt` | shared state access, string helpers, CPU controls, `kernel_main`, command policy |
| `os/serial.vnt` | COM1 initialization and byte/string I/O |
| `os/console.vnt` | VGA text output, cursor, scrolling, combined console, hexadecimal output |
| `os/ps2.vnt` | bounded 8042 polling, keyboard translation, mouse packet assembly |
| `os/interrupts.vnt` | interrupt initialization wrapper and VNT-side exception/IRQ policy |
| `os/boot.S` | Multiboot2 header and transition into 64-bit mode |
| `os/interrupts.S` | IDT setup and the small CPU-required interrupt entry/exit boundary |

This is a real module split, not a set of disconnected sample files. The shell calls the same console and input routines after the split.

## Understand `import`

At the top of `os/kernel.vnt`, the imports look like this:

```vnt
import "serial.vnt"
import "console.vnt"
import "ps2.vnt"
import "interrupts.vnt"
```

Each path is relative to the importing file. Since the root file lives in `os/`, `"console.vnt"` means `os/console.vnt`. These imports are deliberately unaliased, so the module loader combines their declarations into the compilation unit and existing calls such as `console_puts(...)` remain direct calls.

Do not copy this import syntax into a hosted project without reading the module rules first. VNT is experimental, and the freestanding compiler supports a smaller language subset than the hosted compiler.

## Trace one call across files

Follow this path in your editor:

1. In `os/kernel.vnt`, find `kernel_main()`.
2. Find the call to `console_puts("VNTOS> ")`.
3. Open `os/console.vnt` and find `console_puts`.
4. Follow it to `vga_puts` and `serial_puts`.
5. Open `os/serial.vnt` and find `serial_puts`, then `serial_putc`.

One shell operation can cross multiple modules, but the linker still produces one ELF kernel. Modules are source organization; they do not automatically create separate processes, address spaces, or independently loadable binaries.

## Rebuild after reading

From the repository root:

```sh
bash os/build.sh
```

The build script still has one VNT root file: `os/kernel.vnt`. The compiler's module loader follows the imports and includes the imported declarations. The script separately assembles the two architecture-boundary files and links all three object files into `build/kernel.elf`.

If the compiler reports a missing imported file, check the path relative to `os/kernel.vnt`. If it reports an undeclared function, check spelling and whether the function exists in one of the imported modules before moving code around.

## Design rule

A module should own one reason to change. Keep command matching in the kernel, character rendering in the console, and device polling in the input driver. The interrupt module follows the same rule: assembly enters and exits the CPU interrupt frame; VNT decides what the event means.

## Checkpoint

Explain why moving `console_puts` to `console.vnt` does not create a separate executable. Name the two files that contain assembly and explain why neither should contain command parsing.

## Exercise

Add a short comment above one function explaining its contract and limitation. Rebuild the kernel. Do not rename exported functions or change the ABI as part of this exercise.
