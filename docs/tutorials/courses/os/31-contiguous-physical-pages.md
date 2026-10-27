# Contiguous physical-page allocation

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository root `LICENSE`.

## Goal

Add an allocator API for a run of adjacent 4-KiB physical frames. This is a prerequisite for uses that cannot accept unrelated pages, such as some DMA buffers and future kernel memory pools. The implementation belongs in `os/mm/phys.vnt`; it must not silently leak a partial run when allocation or release fails.

## Contract

- `phys_alloc_contiguous_pages(count)` returns the first physical address of a run containing exactly `count` adjacent pages.
- A successful allocation marks every frame allocated, subtracts `count` from the free-page count once, and advances the next-fit cursor beyond the run.
- Invalid counts and exhausted searches return zero without changing allocation bits or counters.
- `phys_free_contiguous_pages(address, count)` validates the entire run before clearing any bit.
- A bad address, invalid count, reserved page, unallocated page, or range extending beyond the supported 1-GiB window rejects the entire free operation.

The API is still single-CPU only. “All-or-nothing” describes failure behavior within one serialized call; it does not make the allocator thread-safe.

## Code example: preflight a candidate run

```vnt
fun phys_range_is_free(first: int, count: int) -> int {
    if count <= 0 || first < 256 || first >= 262144 {
        return 0
    }
    if count > 262144 - first {
        return 0
    }

    let frame: int = first
    while frame < first + count {
        if phys_usable_get(frame) != 1 ||
           phys_allocated_get(frame) != 0 ||
           phys_reserved_get(frame) != 0 {
            return 0
        }
        frame = frame + 1
    }
    return 1
}
```

Frame 256 is the 1-MiB boundary. Frame 262144 is the exclusive end of the first GiB. The subtraction in `count > 262144 - first` checks the bound without first adding an unchecked count to the frame index. Every frame must be usable, unallocated, and not reserved.

## Allocation algorithm

`phys_alloc_contiguous_pages(count)` validates the count, normalizes the next-fit cursor, and searches for a candidate whose entire run passes `phys_range_is_free()`. It first checks candidate starts from the cursor through the highest start that still fits. It then wraps to frame 256 and checks starts below the original cursor. A run is never allowed to wrap physically across the end of the mapped window.

Only after the complete candidate passes does the allocator mark each frame allocated and decrement the cached free count. That order matters: if a candidate contains one unavailable frame, the function must not leave earlier frames allocated.

## Free algorithm: validate first, mutate second

Freeing uses two loops for a reason. The first loop checks every frame for usability, allocation ownership, and reservation status. If any check fails, the function returns before changing state. The second loop clears the allocation bits only after the whole range is known to be releasable. The free counter is then increased by `count`, and the next-fit cursor is rewound if the released run begins below it.

This is a small transaction pattern: **preflight every condition, then commit the mutation**. Do not merge the validation and mutation loops; doing so would permit partial frees.

## Example call

```vnt
let run: i64 = phys_alloc_contiguous_pages(4)
if run == 0 {
    console_puts("Could not allocate four adjacent pages.\\r\\n")
} else {
    # run points to page 0; pages 1–3 follow at 4096-byte intervals.
    if phys_free_contiguous_pages(run, 4) != 1 {
        console_puts("Contiguous-page release failed.\\r\\n")
    }
}
```

For four pages, the byte length is 16384. The returned address is the first page's physical address, not a pointer to a virtual heap object. The current first-GiB identity map makes that physical address directly addressable in the current kernel, but future paging changes must not assume physical and virtual addresses are always identical.

## Self-test coverage

The existing `memtest` command calls `phys_self_test()`. It now also allocates two adjacent frames and verifies:

1. The allocation is page-aligned.
2. The free-page count drops by exactly two.
3. Both frame bits are marked allocated.
4. An unaligned release is rejected without changing the count.
5. A valid release restores the count.
6. Releasing the same run twice is rejected.
7. The final free-page count equals its initial value.

This is a targeted regression check, not a proof of every search-path edge case. In particular, the current self-test does not construct a completely exhausted memory map or force a run to cross the last frame. Those cases require a test seam or a separate allocator harness.

## Verify in QEMU

From the repository root:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Run `memtest` and then `mem` in the shell. The expected self-test message is `Physical allocator self-test: PASS`. Use the GitHub Actions run for the exact commit as build/boot evidence; source inspection alone is not a test result.

## Failure cases

- **Count is zero or negative:** reject before reading any bitmap.
- **Count is too large:** reject before calculating the end of the range.
- **A candidate has a hole:** continue searching without marking any part of it.
- **No complete run exists:** return zero and leave the free count unchanged.
- **A run includes a reserved page:** reject the whole free request and preserve every bit.
- **A range is misaligned:** reject it before converting the address to a frame index.
- **A range exceeds the mapped window:** reject it rather than truncating it.
- **Two CPUs call at once:** still unsupported; the bitmap operations and cursor are not atomic.

## Implementation exercise

1. Trace a three-page allocation and write down the exact counter value after each phase.
2. Explain why a contiguous allocation cannot simply call `phys_alloc_page()` repeatedly: a later failure would leave a partial run allocated.
3. Explain why freeing must validate the whole run before clearing the first bit.
4. Add a test for `count == 0` and verify that the counter and cursor remain unchanged.
5. Design a test seam that can simulate a hole in a candidate range without corrupting the live boot allocator.
6. Keep SMP allocation disabled until the compiler has a documented atomic primitive and the allocator's bitmap, counter, and cursor updates share a lock.

## Acceptance checklist

- [ ] The allocation API rejects invalid counts and out-of-range runs.
- [ ] Successful allocations return adjacent frames and update the count exactly once per frame.
- [ ] Failed allocations do not change allocation bits, free count, or cursor.
- [ ] Free validates the entire run before mutation.
- [ ] Invalid and repeated frees do not partially change state.
- [ ] `memtest` passes in QEMU and the actual CI result is recorded.
- [ ] The English and Finnish course indexes link to this lesson.

## Current limitations

The allocator still covers only the first 1-GiB identity-mapped window and uses bitmaps stored in the kernel image. Contiguous allocation is useful for future subsystems, but it does not implement a general heap, DMA cache-coherency policy, reclaimable reservations, virtual-memory isolation, or SMP-safe locking.

---

[← Previous lesson](./30-physical-allocator-next-fit.md) · [Course index](./README.md) · **Course completed.**
