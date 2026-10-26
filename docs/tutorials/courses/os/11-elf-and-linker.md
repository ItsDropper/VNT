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

## 6. Distinguish file offsets, virtual addresses, and physical loading

An ELF file has a file representation and describes how loadable segments should appear in memory. A file offset tells you where bytes live inside the ELF file; a virtual address tells the CPU-visible address expected by the linked code; the bootloader's loading process determines where the segment is placed physically. These values are related but not interchangeable.

Use `readelf -h -l build/kernel.elf` to inspect the entry point and program headers. Section headers are useful for debugging symbols and organization, but the bootloader primarily uses loadable segments. A section name in the file does not automatically guarantee that the bytes are loaded into memory.

## 7. Why alignment matters

The linker script and input objects must agree on section alignment. The boot protocol has its own header-placement requirements, while page tables and hardware structures have architectural alignment rules. A change that appears to affect only section order can accidentally move the Multiboot2 header outside the loader's search window or violate an expected alignment.

After changing the linker script, compare the old and new ELF headers, program headers, entry point, and section addresses. If a symbol's address changes, search for assembly that assumes a particular address range or mapping.

## 8. Linker symbols are addresses, not allocated objects

A linker-defined symbol can mark the beginning or end of a section or region. Code may take its address to discover a boundary. It is not necessarily a normal variable with storage and an initial value. When reviewing code that references linker symbols, inspect the linker script and the generated symbol table together.

This matters for future BSS clearing, memory boundaries, and physical memory management. Do not infer the size of a region from a symbol name; calculate it from the documented start and end symbols and verify the linker definition.

## 9. BSS and initialization

The BSS region holds zero-initialized storage that does not need to occupy the same amount of literal bytes in the ELF file. The loader and startup contract must ensure that this region has the expected initial contents. Kernel state such as tables and counters relies on its initialization rules; stale memory would be especially dangerous for an IDT or shared device state.

The current bootstrap and linker script form one contract. If you change how BSS is laid out or initialized, verify the actual bootloader behavior and the startup code rather than assuming a hosted C runtime will clear it for you.

## Lab: answer from the artifact

Run `readelf -h -l build/kernel.elf` and `nm -n build/kernel.elf`. Record the entry point, loadable segment ranges, and addresses of the interrupt and kernel entry symbols. Explain which command shows each fact and why none of these checks alone proves a successful boot.
