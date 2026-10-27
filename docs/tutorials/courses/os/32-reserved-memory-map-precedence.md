# Reserved memory-map regions take precedence

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

Make physical-memory initialization conservative when firmware's Multiboot2 memory map contains overlapping entries. An available region must not make a page allocatable if any part of that page is also covered by a reserved or otherwise non-available region.

The implementation is in `os/mm/phys.vnt`. It uses two passes: first mark complete pages from type-1 (available) entries, then clear any page touched by a non-type-1 entry. Kernel and boot-information reservations are applied afterward.

## Why two passes?

A memory map usually describes distinct ranges, but a kernel must not depend on entry order when ranges overlap. If an available entry is processed after a reserved entry and simply sets a bit, a reserved page could accidentally become allocatable. The two-pass design gives non-available entries precedence regardless of their order in the table.

The page-boundary rules differ intentionally:

- **Available region:** include only complete 4096-byte pages. Round the start up and the end down.
- **Reserved region:** reject every 4096-byte page touched by the region. Round the start down and the end up.

This conservative rule can discard a page that is only partially available, but it prevents the allocator from returning a frame that overlaps firmware-owned memory.

## Source map

- `os/mm/phys.vnt`: `phys_mmap_count()`, `phys_mmap_base()`, `phys_mmap_length()`, `phys_mmap_type()`, and `phys_init()`.
- `os/mm/phys.vnt`: usable, allocated, and reserved frame bitmaps.
- `os/kernel/shell.vnt`: `mem` and `memtest` commands.
- `.github/workflows/vntos-boot.yml`: the real build and QEMU boot checks.

## Pass one: mark complete available pages

The existing available-region path rounds inward:

```vnt
start_frame = (base + 4096 - 1) / 4096
end_frame = (base + length) / 4096
if start_frame < 256 {
    start_frame = 256
}
frame = start_frame
while frame < end_frame {
    phys_usable_set(frame, 1)
    frame = frame + 1
}
```

Frame 256 begins at 1 MiB. The upper bound of 262144 is the exclusive frame index for the end of the first GiB. The code clamps each region to that supported window before calculating frame indices.

## Pass two: clear every page touched by a non-available region

```vnt
start_frame = base / 4096
end_frame = (base + length + 4095) / 4096
if end_frame > 262144 {
    end_frame = 262144
}
frame = start_frame
while frame < end_frame {
    phys_usable_set(frame, 0)
    frame = frame + 1
}
```

The full implementation first checks that `base` is inside the supported window and that `length` is positive. It then clamps the length to `limit - base` before calculating the end. That ordering avoids adding an unbounded firmware length to the base address.

The start uses integer division, which rounds down. Adding 4095 before dividing rounds the end up. Consequently, a reserved range from byte 5000 through byte 6000 invalidates the page that contains those bytes, even though the range does not fill an entire page.

Every memory-map entry whose type is not 1 is treated as unavailable for general allocation. This is deliberately conservative: ACPI reclaimable memory and NVS are not handed to the ordinary page allocator without a separate lifecycle policy.

## Why the order matters

Suppose the map contains these entries:

- available: `0x200000–0x600000`
- reserved: `0x300800–0x301000`

The reserved range touches frame `0x300000 / 4096`. The first pass may mark that frame available because the larger type-1 range covers it. The second pass clears the usable bit for that frame. Reversing the input entry order does not change the result.

After both passes, `phys_init()` reserves the low-memory/kernel range and the Multiboot2 information block, then computes the usable and free counters from the final bitmap. The cached counters therefore reflect the conservative result.

## Bounds and failure cases

1. **Region starts at or above 1 GiB:** ignore it; this allocator cannot use it.
2. **Region crosses the 1-GiB limit:** clamp its length before calculating the end.
3. **Zero or negative length:** ignore the entry.
4. **Available region starts mid-page:** the partial first page is not marked usable.
5. **Reserved region ends mid-page:** the final touched page is cleared from the usable bitmap.
6. **Reserved entry overlaps an available entry:** the second pass wins.
7. **Reserved entry appears before the available entry:** the result is unchanged because all reserved entries are processed in the second pass.
8. **Kernel image lies in an available region:** the later kernel reservation still marks its frames allocated and reserved.

## Verification

From the repository root, use the real kernel build and the existing QEMU shell workflow:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Run `mem` and `memtest` in the guest. Existing tests verify allocation/free accounting and that reserved kernel memory cannot be freed. They do not synthesize overlapping Multiboot2 map entries, so do not claim that a dedicated overlap regression test exists. Check the exact GitHub Actions result for the commit containing the change.

A useful future test would feed a synthetic list of memory-map entries into a pure range-normalization helper, then assert that the final usable bitmap is independent of entry order. That test should not mutate the live boot allocator's bitmaps.

## Implementation exercise

1. Explain why available ranges round inward while reserved ranges round outward.
2. Calculate the frame indices for a region beginning at byte 5000 and ending at byte 6000.
3. Explain why clamping must happen before computing `base + length + 4095`.
4. Trace how the final bitmap is converted into `state_get(28)` and `state_get(29)`.
5. Design a synthetic overlap test with the available and reserved entries in both orders.
6. Do not mark ACPI reclaimable memory usable until the kernel has a separate, explicit reclamation policy.

## Acceptance checklist

- [ ] Available entries mark only complete pages.
- [ ] Non-type-1 entries clear every touched page from the usable bitmap.
- [ ] Reserved entries win regardless of map entry order.
- [ ] Region bounds are clamped before end-address arithmetic.
- [ ] Kernel and Multiboot2 information reservations still apply after both passes.
- [ ] Existing `memtest`, boot, and shell checks pass in actual CI/QEMU results.
- [ ] Both course indexes and the preceding lesson link to this lesson.

## Current limitations

This change makes the bitmap construction more conservative; it does not add memory hotplug, reclaim ACPI memory, support physical addresses above 1 GiB, or make allocation SMP-safe. The allocator still uses boot-time bitmaps and a serialized single-CPU access contract.

---

[← Previous lesson](./31-contiguous-physical-pages.md) · [Next lesson →](./33-terminal-history-and-uptime.md) · [Course index](./README.md)
