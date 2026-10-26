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

## Practical refactor checklist

When doing this kind of refactor in your own branch, use this order:

1. Pick one group of related functions; do not move the whole kernel in one edit.
2. Create the new file with the repository's copyright and license header.
3. Move the complete function bodies without changing their instructions.
4. Add one import to the root source file.
5. Search the repository for every call to the moved function.
6. Run `bash os/build.sh` immediately.
7. Boot the resulting ISO and test the path you moved.
8. Review the diff for accidental behavior changes before moving another group.

The refactor in this repository follows that pattern for serial I/O, the console, and PS/2 input. The interrupt module is new behavior, but its policy is still separated from the shell.

## Why not use a giant shared module?

A single `hardware.vnt` file would still couple unrelated concerns. COM1 has transmit-ready status and receive-ready status. VGA has character cells, attributes, and a hardware cursor. PS/2 input has controller status and scan-code state. Interrupt dispatch has CPU-saved state and PIC acknowledgement. Those contracts are different even though they all touch hardware.

The goal is not to maximize the number of files. The goal is to make the dependency direction visible and keep each file understandable. If a module becomes a dumping ground for unrelated helpers, split it only after a concrete boundary appears.

## How to inspect what changed

Use these commands from the repository root:

```sh
git diff --stat
git diff -- os/kernel.vnt os/serial.vnt os/console.vnt os/ps2.vnt
grep -n '^import ' os/kernel.vnt
```

The `git diff` commands show the move and the import list. They do not prove the compiler loaded every module; the build and QEMU run provide that evidence.

## A useful failure distinction

- **File not found:** import path or filename is wrong.
- **Undeclared function:** a declaration was omitted, misspelled, or not included by the loader.
- **Duplicate symbol:** two modules define the same external function.
- **Build passes, behavior changes:** a function was moved with a changed body or hidden dependency.
- **Build and boot pass:** still test the specific device path; a working prompt alone does not prove keyboard or timer behavior.

Keep this checklist in your notes for every future subsystem extraction.
