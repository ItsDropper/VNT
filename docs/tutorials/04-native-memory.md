# Tutorial 4: Native Memory Fundamentals

This lesson introduces pointers and explicit memory operations. It is for learning and testing native code, not for experimenting on important data.

## 1. Understand the distinction

- A value such as `42` is an integer.
- A pointer represents a memory address.
- Dereferencing a pointer accesses the object at that address.
- Allocated memory must be released according to the allocator's contract.
- A dangling pointer refers to storage that is no longer valid.

Invalid memory access can crash the program or corrupt data. The compiler cannot make arbitrary unsafe pointer operations safe automatically.

## 2. Read the project's memory API

Open [the raw-memory guide](../raw-memory.md) and inspect the implementation in `lib/std/mem.vnt`. The API reference and source are authoritative for exact function names and signatures; they may change while VNT is experimental.

A typical safe workflow is:

1. Allocate only the amount of memory required.
2. Check whether allocation succeeded before using the returned pointer.
3. Keep accesses within the allocated range.
4. Stop using the pointer after freeing the allocation.
5. Free each allocation exactly once.

## 3. Use unsafe operations deliberately

VNT supports `unsafe { ... }` blocks for operations that need explicit programmer responsibility. Keep such blocks as small as possible and document the assumptions that make each operation valid.

Do not copy an example involving raw addresses into a different process or machine without understanding its address space and permissions.

## Exercises

1. Read `docs/raw-memory.md` and identify the allocation, read/write, and release operations it documents.
2. Explain why using a pointer after releasing its allocation is invalid.
3. Find an unsafe block in the standard library and write down its preconditions.

Next: [Walk through the VNTOS kernel](05-vntos-kernel-walkthrough.md).
