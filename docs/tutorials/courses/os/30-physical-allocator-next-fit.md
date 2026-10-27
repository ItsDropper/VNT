# Improve physical-frame allocation with next-fit

Copyright (c) 2026 ItsDropper. MIT License; see the repository-root LICENSE.

## Goal

Change the physical page-frame allocator so repeated allocations do not always scan from the 1 MiB boundary. Keep the allocator bounded, preserve its accounting invariants, and test the new cursor behavior. This is a kernel implementation lesson: the code belongs in `os/mm/phys.vnt`, and the cursor storage belongs to the existing bootstrap-owned state table.

## What the allocator owns

The current allocator works inside the bootstrap's identity-mapped first GiB. A frame is 4096 bytes. The usable bitmap says firmware reported the frame as usable; the allocated bitmap says the kernel currently owns it; the reserved bitmap prevents freeing permanent kernel and page-table storage.

`phys_alloc_page()` must return either a 4096-byte-aligned physical address or zero. On success, exactly one usable, previously-free frame becomes allocated and the cached free-frame count decreases by one. `phys_free_page()` must reject unaligned addresses, frames outside the supported window, frames not allocated, non-usable frames, reserved frames, and double frees.

## Why change first-fit?

The old loop started at frame 256 (1 MiB) on every allocation. If many low frames were already allocated, every later request repeatedly read the same bitmap bytes. Next-fit remembers the frame after the last successful allocation, searches upward, then wraps once to frame 256. This improves the common sequential-allocation case without changing the allocator's public contract.

Next-fit is an optimization, not a concurrency primitive. The allocator still uses ordinary bitmap read/modify/write operations and the global free count. Until the compiler and kernel provide a proper atomic and memory-ordering contract, only the existing serialized early-boot/shell paths may use it. Do not call it concurrently from APs or interrupt handlers.

## Source map

- `os/mm/phys.vnt`: memory-map parsing, bitmaps, allocation, free, and `phys_self_test()`.
- `os/arch/x86_64/boot.S`: shared scalar state; slot 176 stores the next-fit frame index.
- `os/kernel/core.vnt`: access to the bootstrap-owned state table.
- `os/kernel/shell.vnt`: `memtest` command that reports the self-test result.
- `os/build.sh`: the authoritative list of assembly files and kernel link inputs.

## Read the implementation

First, follow `phys_init()`. It validates the Multiboot2 memory map, marks usable frames, reserves the kernel image and boot information, counts usable/free frames, and initializes the search cursor to 256. The cursor is an index into 4096-byte frames, not a byte address.

Then follow `phys_alloc_try_frame(frame)`. It rejects an out-of-range index, checks the usable and allocated bitmaps, marks the frame allocated, decrements the free count, advances the cursor, and returns `frame * 4096`. A zero result means that this candidate was not allocated.

Finally, follow `phys_alloc_page()`. It validates the cursor, scans from the cursor to the end of the supported frame range, and wraps from frame 256 up to (but not including) the original cursor. Each candidate is considered at most once per request. If no candidate succeeds, it returns zero without changing the free count.

## Code example: bounded next-fit search

```vnt
fun phys_alloc_page() -> i64 {
    let start: int = state_get(176)
    if start < 256 || start >= 262144 {
        start = 256
        state_set(176, start)
    }

    let frame: int = start
    while frame < 262144 {
        let page: i64 = phys_alloc_try_frame(frame)
        if page != 0 {
            return page
        }
        frame = frame + 1
    }

    frame = 256
    while frame < start {
        let page: i64 = phys_alloc_try_frame(frame)
        if page != 0 {
            return page
        }
        frame = frame + 1
    }
    return 0
}
```

The upper bound 262144 is 1 GiB divided by 4096 bytes. Frame 256 is the first eligible frame because lower memory is reserved. The second loop is the wraparound pass; it stops before `start`, so it never repeats candidates from the first loop. The helper advances slot 176 only after a successful allocation.

## State-table contract

Slot 176 is added after the existing SMP state slots in `boot.S`. Its value is a frame index, and the valid range is 256 through 262143. A zero or out-of-range value is treated as an uninitialized cursor and reset to 256. `phys_init()` resets it after calculating the initial free-frame count.

Do not reuse slot 27: slots 12–27 are already the sixteen legacy IRQ counters. Slots 28 and 29 hold usable/free frame counts; slots 30 onward are already assigned to ACPI, APIC, and SMP state. Keeping the new index documented avoids silent state aliasing.

## Freeing and cursor behavior

When a frame is freed successfully, the free count increases exactly once. If the freed frame is below the current cursor, the cursor moves back to that frame so the next allocation can reuse the newly available lower address without waiting for a full wrap. A failed free must not change either the count or cursor.

The cursor update is not atomic. That is acceptable only under the current serialized usage contract. It is not safe to infer that adding a cursor makes the allocator SMP-safe; the bitmap and free count would still race.

## Test the invariants

Run from the repository root:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

In the VNTOS shell, run `memtest` and then `mem`. `memtest` should report `Physical allocator self-test: PASS`. The self-test verifies that an allocation advances slot 176 to the next frame (or wraps to 256 at the end of the range), that the returned address is aligned, that an unaligned free is rejected, that a valid free succeeds, that a second free is rejected, and that the free count is restored.

If `build/vntos.iso` was not created, do not run the QEMU command as written; install the ISO tools described in `os/README.md` or use the existing `build/kernel.elf` validation workflow. A successful compile alone does not prove the cursor behavior. Check the GitHub Actions result for the exact commit before claiming CI passed.

## Failure cases to reason through

1. **Cursor is zero:** the allocator resets it to frame 256 before scanning.
2. **Cursor is 262144 or higher:** it is invalid and resets to 256.
3. **No free frames exist above the cursor:** the search wraps and checks the lower range exactly once.
4. **All frames are allocated:** both loops finish and return zero; the free count remains unchanged.
5. **A frame is freed below the cursor:** the cursor moves backward to make it eligible on the next search.
6. **A reserved frame is freed:** `phys_free_page()` rejects it before changing any accounting.
7. **Two CPUs allocate at once:** unsupported; both could observe the same clear bit. Keep APs out of general allocator use.

## Implementation exercise

1. Trace the free count through one successful allocation and one successful free.
2. Draw the cursor path for `start = 300`: first scan 300 through 262143, then wrap through 256 to 299.
3. Explain why the second loop uses `frame < start` instead of `frame <= start`.
4. Inspect every state slot used by the kernel before adding a new one.
5. Extend `phys_self_test()` only if the extra assertion can be restored to a clean allocator state on every path.
6. Propose a future SMP-safe design, but do not implement it with `volatile` or unannotated inline assembly. The compiler needs a supported atomic/memory-ordering interface first.

## Acceptance checklist

- [ ] `phys_init()` initializes the cursor after the memory map is accepted.
- [ ] A successful allocation advances the cursor and decrements the free count once.
- [ ] A failed allocation returns zero without changing the free count.
- [ ] Search wraps at most once and never examines a candidate twice per request.
- [ ] Freeing a valid page increments the count once and updates the cursor only when appropriate.
- [ ] Invalid and double frees leave allocator state unchanged.
- [ ] `memtest` passes in QEMU and the result is recorded from the actual run.
- [ ] The English and Finnish course indexes both link to this lesson.

## Current limitations

This allocator covers only the low 1-GiB identity-mapped physical window, uses bitmaps stored in the bootstrap image, and has no general heap or SMP lock. The next-fit cursor reduces repeated scanning; it does not solve fragmentation, provide contiguous multi-page allocations, reclaim reserved pages, or permit concurrent access.

---

[← Previous lesson](./29-smp-synchronization-contract.md) · [Course index](./README.md) · **Course completed.**
