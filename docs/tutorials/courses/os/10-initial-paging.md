# Lesson 10 — Understand the Initial Page Tables

**Goal:** understand the first address mapping and its limits.

## 1. What paging does

Paging translates virtual addresses used by code into physical addresses in RAM or other mapped regions. It also provides mechanisms for permissions and isolation. The CPU walks page-table structures to perform the translation.

Paging is not the same as allocating memory. A mapping says how an address translates; an allocator decides which physical pages are available and may be assigned. VNTOS has an initial mapping but not a general physical-memory allocator.

## 2. Four-level page tables

The x86-64 setup here uses four-level paging for the mapped region:
- PML4: the top-level table;
- PDPT: the next-level table;
- page directory: contains large-page entries;
- 2 MiB page: the actual mapping size used by this bootstrap.

The bootstrap connects the first three structures. It fills 512 page-directory entries, each mapping 2 MiB. Since (512 	imes 2	ext{ MiB} = 1	ext{ GiB}), the initial map covers the first 1 GiB of address space.

## 3. Identity mapping

An identity map maps a virtual address to the same-numbered physical address. This makes early execution simpler because code and data can continue to be addressed at their loaded physical locations while paging is enabled.

The current page-directory loop computes each 2 MiB base address, then adds flags for a present, writable, large-page mapping. The low 32 bits of each 64-bit entry are written with the base and flags; the high 32 bits are cleared. That is sufficient for this deliberately limited first-GiB mapping, but it is not a general implementation for arbitrary physical addresses.

## 4. The important limitation

The shell's `mem` command prints linker-defined kernel boundaries and the configured identity-mapped range. It does **not** discover installed RAM or prove that every mapped address is usable RAM. Real machines reserve address ranges for firmware and devices; a robust kernel must parse a trusted memory map and exclude reserved regions before allocating pages.

This distinction is essential. Never interpret “mapped” as “free to use.”

## 5. What should come next in a real kernel?

A safe memory subsystem normally needs:
1. parse bootloader-provided memory information;
2. classify usable and reserved physical ranges;
3. reserve the kernel image, bootstrap tables, stacks, and other essential structures;
4. build a physical-page allocator;
5. create and manage virtual mappings with deliberate permissions;
6. add fault handling and tests.

Those features are future work, not hidden capabilities of the current VNTOS kernel.

## Checkpoint

Explain the difference between a virtual address, a physical address, a page-table mapping, and an allocated page.

## Exercise

Find the loop labelled `.Los_map_2m` in `os/boot.S`. Explain why it loops 512 times and why each entry advances by 2 MiB. Do not change the page-table flags as an experiment on physical hardware.