# Lesson 26 — Physical Memory and Page Frames

**Goal:** understand how VNTOS turns the Multiboot2 memory map into a bounded
physical page-frame allocator, and verify that allocation and freeing preserve
its invariants.

This is an early physical-memory subsystem, not a complete virtual-memory
manager. It works only inside the first 1 GiB, which the bootstrap currently
identity-maps. It is deliberately not SMP-safe.

## 1. Vocabulary

- **Physical address:** an address on the machine's physical memory map.
- **Page frame:** a fixed-size physical memory unit. VNTOS uses 4-KiB frames.
- **Memory map:** bootloader-provided ranges with types such as usable RAM or
  reserved memory.
- **Reserved frame:** a frame the allocator must never return or free, such as
  the kernel image or the bootloader's information block.
- **Bitmap:** one bit per frame, used to record a property compactly.
- **Identity mapping:** a virtual address equals its physical address for the
  mapped range. It does not mean all physical memory is mapped.

There are 262,144 4-KiB frames in 1 GiB. A bitmap for that many frames takes
32,768 bytes (32 KiB).

## 2. Follow the boot information

GRUB enters the Multiboot2 kernel with a boot magic value in EAX and a pointer
to the information structure in EBX. The first instructions in
`os/arch/x86_64/boot.S` preserve both values before using those registers for
CPU setup.

The parser in `os/mm/phys.vnt` validates the handoff and walks aligned
Multiboot2 tags using the freestanding backend's explicit unsafe memory
intrinsics. It checks tag boundaries, entry sizes, and the 1-GiB identity-map
limit before exposing memory-map fields. Bitmap bit operations also execute as
compiler-generated VNT; bootstrap assembly reserves only the bitmap storage.
The compiler emits native loads/stores for `mem_read_u8/u32/u64` and
`mem_write_u8/u32/u64`, and requires those calls to appear inside an
`unsafe` block.

## 3. Read `os/mm/phys.vnt`

The allocator uses three bitmaps:

1. **Usable:** the frame belongs to a type-1 usable memory-map region.
2. **Allocated:** the frame is already allocated or unavailable for allocation.
3. **Reserved:** the frame is owned by the kernel or bootloader and must never
   be released by the ordinary page-free API.

The VNT initializer considers only usable regions, clips them to the first
1 GiB, rounds ranges inward to whole 4-KiB frames, and ignores low memory below
1 MiB. It then reserves the complete low-memory/kernel image range and the
Multiboot2 information block. The kernel image range includes its page tables,
bootstrap stack, and the allocator bitmaps themselves.

The third bitmap matters: an allocated bit alone cannot tell an ordinary
allocation from a permanent reservation. Without a separate reserved bit,
`phys_free_page` could accidentally release a page occupied by the kernel.

## 4. The public operations

- `phys_init()`: parse the map, mark usable frames, reserve boot-owned ranges,
  and initialize the cached counts. Returns zero if the map is invalid or
  missing.
- `phys_alloc_page()`: first-fit scan from 1 MiB upward. Returns an aligned
  physical address, or zero when no frame is available.
- `phys_free_page(address)`: frees only an aligned, usable, allocated,
  non-reserved frame. Invalid addresses and double frees return zero.
- `phys_count_usable_pages()` and `phys_count_free_pages()`: return cached
  counts. Allocation and successful free adjust the free count.

The first-fit allocator is simple and easy to audit, but allocation is O(n) in
the number of frames. It does not map new memory, create a heap, or support
physical addresses above 1 GiB. The counters and bitmaps are not synchronized
for concurrent CPUs; do not use this allocator from SMP code yet.

## 5. Build and boot

From the repository root:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

If the ISO tools are installed, the build creates `build/vntos.iso`. In the
serial shell, run:

```text
mem
memtest
```

The `mem` command prints kernel bounds, the current identity-map limit, and
usable/free 4-KiB page counts. The counts are hexadecimal because the console
uses the existing `console_hex` routine.

The `memtest` command allocates a page, checks alignment, frees it, verifies
that a second free is rejected, attempts to free the kernel start frame, and
checks that the free-page count returns to its original value. GitHub Actions
also runs this command in QEMU and asserts the self-test's PASS marker.

## 6. Failure cases to understand

- A missing or malformed Multiboot2 memory map must not produce a fake free
  page count; initialization returns failure.
- A region beyond 1 GiB is not managed, even if the machine has more RAM.
- Reserved frames can have their usable bit set because the physical map says
  they are RAM; the separate reserved bitmap still prevents them being freed.
- A page-free request must be aligned and inside the supported window.
- A double free must not increment the free count twice.
- A successful ELF link does not prove that the memory map was parsed or that
  allocation and freeing work. The QEMU shell test exercises those behaviors.

## Checkpoint

Explain why the allocator needs separate usable, allocated, and reserved
bitmaps. Then trace the following path in the source:

`Multiboot2 registers → boot.S capture → phys.vnt tag parser → phys_init() →
phys_alloc_page() → phys_free_page()`.

## Exercise

Extend `memtest` with one additional invalid-free case. Keep the free-page
count unchanged and make the QEMU workflow assert the expected result. Do not
enable SMP or preemptive scheduling until allocation/free has a synchronization
contract and the VNT/assembly ABI no longer relies on incidental stack offsets.

## Current limitations

The allocator is single-core, bitmap-based, first-fit, and restricted to the
first 1-GiB identity map. There is no general virtual-memory manager, heap,
page-fault recovery, buddy allocator, or per-CPU allocation state yet. Those are
separate milestones rather than implied capabilities.
