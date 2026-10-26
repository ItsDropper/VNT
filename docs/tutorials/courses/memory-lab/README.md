# Build a Memory Arena and Inspector in VNT — 20 Lessons

A beginner-friendly, hands-on course that teaches low-level memory by building a multi-file arena allocator and inspector. Each lesson explains a concept, gives a concrete task, and asks you to verify an invariant rather than merely copy code.

## Requirements and safety

You need a working VNT compiler and terminal. Read [the raw-memory guide](../../../raw-memory.md) and inspect `lib/std/mem.vnt` before using APIs. VNT is experimental; the source in your checkout is authoritative.

All examples are hosted programs. **Never write to physical memory, kernel addresses, device registers, or disks.** Raw pointers can crash or corrupt the process. Only access memory owned by a live allocation or another documented valid region.

## Lessons

- [00 — Start here](00-start-here.md)
- [01 — The process memory model](01-the-process-memory-model.md)
- [02 — Pointers and byte offsets](02-pointers-and-byte-offsets.md)
- [03 — The unsafe contract](03-the-unsafe-contract.md)
- [04 — Allocation failure](04-allocation-failure.md)
- [05 — Alignment and padding](05-alignment-and-padding.md)
- [06 — Overflow-safe bounds checks](06-overflow-safe-bounds-checks.md)
- [07 — Your first reservation](07-your-first-reservation.md)
- [08 — Encapsulate allocator state](08-encapsulate-allocator-state.md)
- [09 — Reset and lifetime](09-reset-and-lifetime.md)
- [10 — Destroy and release](10-destroy-and-release.md)
- [11 — Fixed-width values](11-fixed-width-values.md)
- [12 — Copy and fill](12-copy-and-fill.md)
- [13 — Measure internal waste](13-measure-internal-waste.md)
- [14 — Debug memory failures](14-debugging.md)
- [15 — Build a test matrix](15-testing.md)
- [16 — Deterministic stress testing](16-stress-testing.md)
- [17 — Benchmark honestly](17-benchmarking.md)
- [18 — Security and design review](18-security-review.md)
- [19 — Graduation: memory inspector](19-graduation-project.md)

## Multi-file example project

The implemented companion project is in [`examples/courses/memory-lab/`](../../../../examples/courses/memory-lab/README.md), with separate allocator, inspection, reporting, entry point, and test files. The course then asks you to extend it with stronger tests and design limitations.

## Workflow

1. Read the lesson and inspect the standard-library source.
2. Make one small change and compile.
3. Run the example and compare the result with the expected invariant.
4. Add a regression test for each bug fixed.
5. Record differences between the lesson and your compiler version.

You finish when you can explain pointer validity, alignment, overflow-safe bounds checks, allocation failure, reset versus free, endianness, and why unsafe blocks do not prove memory safety. This learning arena is not production-ready: it has no individual free, concurrency control, coalescing, or guard pages.