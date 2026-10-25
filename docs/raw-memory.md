# Raw pointers and memory

VNT's initial native memory API provides an explicit `ptr` type and allocation/read/write primitives for low-level runtime work.

## API

Import the standard module:

```vnt
import "../lib/std/mem.vnt" as mem

unsafe {
    let storage: ptr = mem.alloc(16)
    let wrote: bool = mem.write_u32(storage, 0, 305419896)
    print(mem.read_u32(storage, 0))
    let released: bool = mem.free(storage)
}
```

Available operations:

- `mem.alloc(size)`: allocate zero-initialized storage; size must be a positive signed `int`.
- `mem.realloc(pointer, size)`: resize an allocation. On success, use the returned pointer and stop using the old pointer. If allocation fails, the returned pointer is null and the original allocation remains valid.
- `mem.free(pointer)`: release an allocation and null the passed pointer value.
- `mem.is_null(pointer)`: check whether a pointer value is null; use this to handle failed allocation/reallocation without discarding the original allocation.
- `mem.offset(pointer, bytes)`: produce an address offset by a signed byte count.
- `mem.read_u8` / `mem.read_u32` and `mem.write_u8` / `mem.write_u32`: unaligned-safe access implemented with `memcpy`.
- `mem.copy(destination, source, length)`: overlap-safe byte copy.
- `mem.set(destination, byte_value, length)`: fill a byte range.

## Safety contract and current limits

These are raw native memory operations. The caller must ensure that each address refers to a live allocation or valid mapped region, offsets stay within the region, ownership is respected, and memory is not freed while still in use. The runtime cannot determine allocation bounds from a raw pointer; invalid access can crash or corrupt the process. Do not pass arbitrary addresses to these functions.

VNT parses `unsafe { ... }` blocks and `unsafe fun` declarations. Direct raw memory builtins and calls to functions marked `unsafe` are rejected outside an unsafe block. The standard-library `mem` helpers are marked `unsafe`, so callers must opt in at the call site. VNT supports `asm volatile("instruction")` inside `unsafe` blocks for simple x86-64 GNU assembler templates. This initial form has no input/output operands, register constraints, clobber list, or compiler memory-clobber support; the template is emitted directly into GNU assembler output. Use AT&T syntax and verify generated code. VNT still needs a proper pointer/integer-width model (`usize`/`isize`) and 64-bit integer loads and stores. Current allocation sizes and offsets use signed 32-bit `int`; `read_u32` currently returns VNT's signed integer representation. These constraints make this a stepping stone, not yet sufficient for a production kernel or hardware driver.

The native target is x86-64, but raw pointer APIs alone do not provide a freestanding target: OS creation also needs a freestanding compilation/link path, startup/ABI support, inline or external assembly, and platform-specific boot/runtime integration.
