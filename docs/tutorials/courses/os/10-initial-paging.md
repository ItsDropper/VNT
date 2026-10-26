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

## 6. Walk one virtual address through the current mapping

For a 2 MiB page mapping, the low 21 bits of the virtual address select a byte within the page. The remaining address bits select the page-table structures and the physical page frame. Because the bootstrap identity-maps the first 1 GiB, an address in that range resolves to the same numeric physical address under the current setup.

Identity mapping is a convenience, not memory protection. It does not stop kernel code from overwriting another kernel object, and it does not distinguish trusted code from untrusted code. It only describes how virtual addresses translate to physical addresses.

## 7. Why the first GiB and 2 MiB pages?

The bootstrap needs a simple mapping for early code, data, the stack, and legacy devices such as VGA text memory. Large pages reduce the number of page-table entries needed for this early map. The trade-off is coarse granularity: the initial design cannot independently map or unmap every 4 KiB region.

Do not assume all memory below 1 GiB is RAM. Some physical ranges may be reserved for devices or firmware. The current identity map is a minimal boot arrangement; a future physical memory manager must consult a firmware-provided memory map and avoid reserved regions.

## 8. What a real memory manager must add

A page allocator needs a trustworthy list of usable physical frames, bookkeeping for allocated and free frames, alignment rules, overflow-safe range checks, and a policy for reserved memory. A virtual-memory manager additionally needs page-table creation, permissions, mapping/unmapping rules, and TLB invalidation when required.

A heap allocator is a separate abstraction built on top of memory supplied by a lower layer. It cannot safely be added by simply treating any address as available. VNTOS currently has no general physical-page allocator or heap, so avoid writing kernel code that assumes dynamic allocation exists.

## 9. Paging and interrupts interact

The CPU uses virtual addresses for interrupt handlers and the IDT base while paging is enabled. Therefore the IDT, entry stubs, stack, and handler code must remain mapped when an interrupt arrives. The current identity map keeps the initial low kernel image and its data reachable under the existing layout. A higher-half kernel or separate address spaces would need to revisit these assumptions.

## Lab: inspect an address range

Choose the VGA text buffer address `0xB8000`. Explain why it lies inside the bootstrap's mapped range, why that makes the console access possible under the current identity map, and why the same numeric address is not guaranteed to work after a future mapping redesign.

## 10. Inspect the table entries as encoded data

A page-table entry is a bit field, not just an address. Some bits describe presence and permissions; other bits encode the physical frame or page size. A large-page entry must set the correct page-size flag and encode an address aligned to the large-page boundary. Accidentally treating a 2 MiB mapping as a 4 KiB mapping changes how the CPU interprets the entry.

When reading the bootstrap, decode the entry fields and compare the address alignment with the page size. Avoid changing constants until you can explain which architectural field each bit controls. The processor manual is the authority for the bit layout.

## 11. Permission bits and future isolation

The current early mapping is intentionally permissive and intended for a tiny kernel. A production memory design would need to decide which pages are writable, executable, or user-accessible. Write protection and execute-disable features can help enforce those decisions, but only if the kernel's mappings and code layout are designed consistently.

Do not claim that paging automatically provides process isolation. Isolation requires separate address-space policy and controlled transitions, not just turning paging on.

## Lab: review the current map

Draw the path from CR3 to the page-table structures used by the current identity map. Mark the region covered by the 1 GiB mapping and the location of VGA text memory. Then list three features a physical/virtual memory manager must add before this map could support isolated processes.

## 12. Avoid unsafe assumptions about mapped memory

A virtual address can be numerically small and still be invalid if the active page tables do not map it. Conversely, a mapped address may refer to device memory rather than ordinary RAM, so normal caching and access assumptions may not apply. The current bootstrap map is intentionally broad to get the kernel running; it is not a validation routine for arbitrary pointers.

Before adding a new memory access, establish the address range, mapping, access width, and owner of the region. A future memory manager must track which frames are available and which are reserved; the identity map alone provides no such ownership information.

## 13. Review paging changes as a chain of invariants

The page-table root must be aligned and reachable, each table pointer must refer to valid memory, the entry flags must match the intended page size, and the final mapping must cover the code and data used immediately after paging is enabled. A correct entry at one level does not compensate for a broken pointer at another level.

For every paging patch, test the unchanged boot path first, then add one address-range check or controlled experiment. Keep a diagram of the mapping that changed and record whether the CPU reset, faulted, or reached the next marker.
