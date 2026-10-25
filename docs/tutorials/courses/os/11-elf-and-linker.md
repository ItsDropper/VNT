# Lesson 11 — ELF, Sections, and the Linker Script

**Goal:** understand how compiled code gets addresses that the bootloader can load.

## 1. Why linking is necessary

The assembler creates object files containing machine code and data, plus metadata about symbols and sections. The linker combines those objects into an executable image and resolves symbol references. A kernel needs predictable addresses because its bootstrap, code, data, and page tables must agree about where things live.

## 2. ELF in plain language

ELF is a common executable-file format on Unix-like systems. It contains a header, section metadata, symbols, and program headers describing loadable segments. Bootloaders generally need the loadable information, not just the names of source files.

Use these commands after a successful build:

```bash
readelf -h build/kernel.elf
readelf -l build/kernel.elf
readelf -S build/kernel.elf
```

The header identifies the architecture and entry point. Program headers describe loadable segments. Section headers describe parts such as `.text`, `.rodata`, `.data`, and `.bss`.

## 3. Read `os/linker.ld`

The script:
- selects the ELF64 x86-64 output format;
- declares `boot32_entry` as the entry symbol;
- defines loadable text and data program headers;
- begins the kernel layout at address `0x00100000` (1 MiB);
- preserves the Multiboot2 header;
- places bootstrap code, normal code, read-only data, data, and BSS;
- defines `__kernel_start` and `__kernel_end`;
- discards selected metadata sections that are not needed in this image.

The `ALIGN` expressions place sections at required boundaries. The `KEEP` expression prevents the linker from dropping the Multiboot2 header as unreferenced data.

## 4. Initialized and uninitialized data

- `.text` contains executable instructions.
- `.rodata` contains read-only data such as constants and string literals.
- `.data` contains initialized writable data.
- `.bss` reserves zero-initialized storage and usually does not need every zero byte stored in the file.

The bootstrap's page tables and stack are reserved in `.bss`. The linker script's `NOLOAD` marker means that region is represented as memory to reserve rather than ordinary file bytes.

## 5. Linker symbols

The symbols `__kernel_start` and `__kernel_end` are defined by the linker script, not ordinary VNT variables. The current kernel uses inline assembly to obtain their addresses. The `mem` command displays these values as diagnostic information.

Those addresses are not a memory map. They only mark the linked kernel's range.

## Checkpoint

Explain why the kernel needs a linker script and why the Multiboot2 header is explicitly retained.

## Exercise

Compare the ELF entry shown by `readelf -h` with the `ENTRY(boot32_entry)` line in `os/linker.ld`. Confirm that they agree. If your tool output differs, stop and investigate before booting.