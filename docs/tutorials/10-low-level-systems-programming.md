# Tutorial 10: Low-Level Systems Programming in VNT

This course examines the current native compiler and the boundary between ordinary native programs and systems software. VNT is experimental: the current compiler emits x86-64 assembly and links a **hosted** executable against a C runtime. Raw pointers and inline assembly are available, but VNT does **not** yet provide a freestanding compiler/linker pipeline or a complete kernel ABI.

Feature labels used below:

- **Supported** — present in the repository and exercised by tests.
- **Experimental** — implemented, but with important constraints or incomplete safety guarantees.
- **Not implemented yet** — a design goal, not a working VNT capability.

## 1. How native compilation works

**Supported.** The current path is:

1. Source text is read by the project/module loader.
2. The lexer produces tokens.
3. The parser builds an AST.
4. Semantic analysis checks types and unsafe contexts.
5. AST nodes are lowered to the flat, index-addressed HIR.
6. HIR optimization and validation run.
7. The x86-64 backend emits GNU assembler source.
8. GCC assembles and links that source with `src/compiler/native_runtime.c`, `native_net.c`, and `native_async_net.c` to make a hosted executable.

The compiler itself is written in C. Relevant implementation: [lexer](../../src/lexer.c), [parser](../../src/parser), [type checker](../../src/compiler/typecheck.c), [HIR definitions](../../include/vnt/ir.h), [HIR lowering](../../src/compiler/ir_lower.c), [x86-64 backend](../../src/compiler/x86_backend.c), and [native runtime](../../src/compiler/native_runtime.c).

Build on Windows from the repository root:

```powershell
.\build.ps1
.\vnt.exe --dump-ir examples\global_variables.vnt
.\vnt.exe --compile examples\global_variables.vnt -o global_variables.exe
.\global_variables.exe
```

On Linux, use the repository's compiler source list in [quick-language CI](../../.github/workflows/quick-language-tests.yml) to build the compiler with GCC. Then use the same CLI form, e.g. `./vnt --dump-ir examples/global_variables.vnt` and `./vnt --compile examples/global_variables.vnt -o global_variables`. The compiler prints the assembly filename and keeps the generated `.s` file beside the executable.

**Important distinction:** `--dump-ir` dumps HIR; it is not an assembly dump. Inspect the generated `.s` file to see assembly. The compiler's output/link step invokes GCC and the hosted C runtime; this is not a freestanding build.

## 2. Memory representation

**Supported concepts; limited language primitives.** Memory is a sequence of bytes. Hexadecimal is a convenient way to describe bit patterns: decimal 305419896 is `0x12345678`. On the x86-64 target, ordinary integer representation is little-endian, so the least-significant byte is stored at the lowest address. The current memory API deliberately uses `memcpy` in its 8- and 32-bit accessors to avoid alignment and strict-aliasing problems in the C runtime.

Alignment is the address constraint some types or instructions impose. Unaligned access may be supported by a particular instruction/CPU, but it must not be assumed for every architecture or device. A pointer is an address; pointer width on the current x86-64 target is 64 bits, but that does **not** mean VNT's `int`, offsets, or allocation sizes are 64-bit. Current VNT `int` values and memory offsets are signed 32-bit.

Stack memory is typically used for function call state and local temporaries. Heap memory is dynamically allocated. VNT's `mem.alloc` currently delegates to the hosted C heap. It does not allocate physical pages, map kernel memory, or provide a kernel allocator.

## 3. Pointers and unsafe blocks

**Experimental.** VNT has a `ptr` type, `unsafe { ... }` blocks, and `unsafe fun` declarations. The API is in [lib/std/mem.vnt](../../lib/std/mem.vnt); runtime implementations are in [native_runtime.c](../../src/compiler/native_runtime.c). Regression coverage includes [unsafe memory](../../tests/unsafe_memory.vnt), [unsafe block](../../tests/unsafe_block.vnt), and compile-failure tests for unsafe-context enforcement.

The following example uses the API as currently implemented. Save it in the repository root so the import resolves as written:

```vnt
import "lib/std/mem.vnt" as mem

unsafe {
    let storage: ptr = mem.alloc(8)
    if mem.is_null(storage) {
        print("allocation failed")
    } else {
        let ok: bool = mem.write_u32(storage, 0, 305419896)
        if ok {
            print(mem.read_u32(storage, 0))
            let second: ptr = mem.offset(storage, 4)
            let wrote: bool = mem.write_u8(second, 0, 255)
            print(mem.read_u8(storage, 4))
        }
        let released: bool = mem.free(storage)
    }
}
```

Expected output on successful allocation and writes:

```text
305419896
255
```

The `bool` locals record operation results; production code should check them before relying on the result. Do not read from the allocation after freeing it.

Available operations are `alloc(size: int)`, `realloc(pointer, size)`, `free(pointer)`, `is_null(pointer)`, `offset(pointer, bytes)`, `read_u8`, `read_u32`, `write_u8`, `write_u32`, `copy`, and `set`. Sizes and offsets are signed 32-bit integers. Allocation sizes must be positive. A failed allocation returns a null pointer. On failed reallocation, retain the original pointer; only replace it after confirming the returned pointer is non-null.

An unsafe block only marks the code as requiring the programmer's explicit acceptance of risk. It does **not** insert bounds checks, extend object lifetimes, prevent use-after-free, validate arbitrary addresses, or make an invalid pointer safe. Keep track of allocation size, ownership, and lifetime yourself. Never pass arbitrary hardware addresses to the hosted allocator API.

## 4. Integer widths and ABI

**Supported:** VNT has `int`, `float`, `bool`, and `ptr` source types and emits x86-64 native code. **Not implemented yet:** genuine `usize` and `isize`, general 64-bit integer literals/arithmetic, and 64-bit memory load/store builtins. Do not use these type names as if they were available; `int` is not a synonym for pointer-sized integer.

The backend currently emits Microsoft x64 calling sequences; the C runtime provides an ABI compatibility boundary on Unix-like x86-64 builds. ABI rules specify where arguments and return values travel, which registers a callee must preserve, stack alignment, and other call-boundary requirements. For example, x86-64 has 64-bit general-purpose registers such as RAX, RBX, RCX, RDX, RSI, RDI, RSP, and RBP. Register names alone do not define a complete calling convention.

Signedness, width, extension, truncation, overflow, and pointer-width conversions need explicit compiler semantics before VNT can safely use integer values as general addresses or sizes. This remains a compiler implementation priority.

## 5. Inline assembly

**Experimental.** The supported syntax is the simple form below, inside an unsafe context:

```vnt
unsafe {
    asm volatile("nop")
}
```

This emits an x86-64 GNU assembler template. It has no declared inputs, outputs, register constraints, clobber list, or compiler memory clobber. Use only instructions valid for the target and verify the generated assembly. See [AST representation](../../include/vnt/ast.h), [statement parser](../../src/parser/statements.c), and [backend](../../src/compiler/x86_backend.c).

A `nop` does not create a synchronization barrier or make a kernel routine safe. The compiler does not know which registers or memory an arbitrary template changes. Hardware ordering and compiler reordering are separate concerns; correct synchronization may require specific instructions **and** compiler-visible constraints. Operand-aware inline assembly is not implemented yet. For nontrivial assembly, an external assembly file with a documented ABI is easier to audit than pretending this minimal form is a complete inline-assembly interface.

## 6. Native interfaces and system calls

**Hosted native execution is supported; direct kernel-mode operations are not.** A VNT executable runs as a process under an operating system. The OS configures its address space and permissions. User-mode code cannot simply execute privileged instructions, access arbitrary physical memory, or install hardware interrupt handlers. Those operations require kernel privileges and platform-specific setup.

A system call is a controlled transition from user mode to the kernel through an OS-defined ABI. It is not the same thing as calling a C runtime function. The current compiler links a hosted runtime and does not provide a stable VNT system-call intrinsic or freestanding syscall ABI. Use the existing standard-library/runtime interfaces for ordinary applications; do not present them as kernel APIs.

## 7. Freestanding binaries and booting

**Not implemented yet.** VNT currently links through GCC with hosted runtime and platform libraries. There is no supported `--target=freestanding` mode, VNT startup object, kernel runtime profile, documented stable freestanding ABI, or end-to-end bootable VNT image pipeline. Commands for such a mode would be fictional today, so this section intentionally provides no fake build command.

A real implementation needs at least: a freestanding code-generation mode; no accidental libc/process/network dependencies; an explicit calling convention; startup/entry-point rules; a linker script; object/ELF inspection; and a test that boots or runs under a controlled emulator. A boot experiment must define its boot protocol (for example, a documented multiboot protocol or UEFI handoff), stack setup, CPU mode, memory map, and serial/debug output. QEMU success must be demonstrated by an automated test, not inferred from successful compilation.

## 8. Drivers and kernel architecture

**Conceptual roadmap, not current VNT driver support.** Hardware drivers often involve:

- **MMIO:** device registers mapped into an address space with device-specific access width and ordering rules.
- **Interrupts:** CPU and interrupt-controller configuration, handler entry/exit conventions, and synchronization with normal code.
- **DMA:** device-visible buffers, physical/IO virtual addresses, cache coherency, mapping and lifetime management.
- **PCI:** configuration-space enumeration, BAR sizing, resource assignment, and device identification.
- **Initialization and HALs:** separating device-specific register operations from common kernel interfaces.

VNT can currently express ordinary control flow, native functions, raw pointer values, basic 8-/32-bit memory access, and a minimal x86-64 assembly statement. It cannot yet provide a complete freestanding runtime, 64-bit typed memory access, a safe MMIO abstraction, interrupt ABI support, DMA allocation/mapping, or a demonstrated bootable kernel. Do not use the hosted heap for MMIO or DMA. These require dedicated compiler/runtime and OS integration.

## 9. Hands-on labs

### Lab A — Inspect a value's bytes

Use the memory example in section 3 to store `0x12345678` and read its first four bytes individually with `read_u8` at offsets 0–3. On the current x86-64 target, the expected byte sequence is `120`, `86`, `52`, `18` (hexadecimal `78 56 34 12`). Keep all accesses within the allocated eight bytes and free the allocation once.

### Lab B — Implement a byte copy

Allocate two buffers of at least eight bytes. Write known values to the source using `write_u8`, then call `mem.copy(destination, source, length)`. Check the returned boolean and read back each destination byte. Do not pass invalid pointers or a length greater than either buffer. The current runtime's copy operation is overlap-safe, but that does not remove the need for valid bounds.

### Lab C — Read generated assembly

Compile `examples/global_variables.vnt` and open the generated `.s` file. Find the function entry, argument setup, runtime calls, labels, and return sequence. Compare it with `--dump-ir`. Write down one HIR operation and the assembly sequence that implements it; exact register allocation can change between compiler revisions.

### Lab D — Add a memory builtin with regression tests

Trace an existing builtin such as `mem_read_u32` through the native runtime implementation, builtin/type handling in the compiler, backend emission, and [unsafe-memory tests](../../tests/unsafe_memory.vnt). Add tests for success and invalid input handling before documenting the API. Keep pointer and length validation explicit. Do not claim a new operation is supported until the compiler and runtime agree on its type and ABI.

### Lab E — Build and inspect a native executable

Compile a small source file using `--compile`, run it, inspect the generated `.s` file, and use an available object inspection tool (such as `objdump -d` on Linux or the corresponding toolchain utility on Windows) to inspect the final executable. This is a **hosted executable**, not a kernel image.

### Lab F — Freestanding smoke test

**Blocked until the freestanding target exists.** Once implemented, add a tiny no-libc entry point, linker script, ELF inspection step, and automated QEMU boot test. The test must verify a deterministic signal such as a serial marker and fail if the emulator exits or times out without that marker.

## Debugging checklist and common mistakes

- If parsing fails, reduce the source to a minimal example and inspect the actual grammar in `src/parser/`.
- If type checking fails, compare the declared type and initializer; VNT does not silently convert incompatible explicit annotations.
- If a memory test crashes, audit pointer validity, allocation length, offset arithmetic, lifetime, and ownership before blaming code generation.
- If generated assembly fails to assemble, verify the x86-64 GNU assembler syntax and the emitted `.s` file.
- If the native linker fails, remember that the current toolchain expects GCC and the hosted runtime sources/libraries.
- If a feature is described as freestanding, 64-bit, or kernel-ready, demand a compiler test plus a runtime/emulator test. Documentation and successful hosted linking are not proof.

## Further reading

- [Compiler architecture](../compiler-architecture.md)
- [Raw memory API and safety limits](../raw-memory.md)
- [API reference](../api-reference.md)
- [Unsafe memory regression test](../../tests/unsafe_memory.vnt)
- [Unsafe context rejection test](../../tests/unsafe_context_rejected.vnt)
- [Inline assembly context rejection test](../../tests/asm_context_rejected.vnt)
- [Quick language CI workflow](../../.github/workflows/quick-language-tests.yml)
