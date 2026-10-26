# Lesson 04 — Learn Just Enough VNT for Kernel Code

**Goal:** recognize the VNT syntax used by the kernel before reading its logic.

This lesson is a focused refresher, not a complete VNT language specification. VNT is experimental, so check current examples and compiler behavior when in doubt.

## 1. Functions and return values

A function is declared with `fun`:

```vnt
fun add(a: int, b: int) -> int {
    return a + b
}
```

The parameters have types and the arrow introduces the return type. Kernel code groups work into functions such as `serial_init`, `serial_puts`, and `kernel_main`.

## 2. Variables and types

The kernel uses declarations like:

```vnt
let index: int = 0
let handled: int = 0
```

It also uses `bool` for true/false results and `i64` for 64-bit integer values. Do not assume that a type named `int` has the same width as a pointer on every target. The freestanding backend and current kernel source have specific supported types.

## 3. Conditions and loops

```vnt
if index < 10 {
    index = index + 1
}
```

```vnt
while index < 10 {
    index = index + 1
}
```

The shell uses a loop to wait for input repeatedly. If the loop condition never becomes false, it continues indefinitely. In a kernel, an intentional infinite loop can be normal; an accidental one can make the system appear frozen.

## 4. Strings and indexing

The kernel compares command strings with `str_eq` and reads characters using indexing, for example `text[index]`. The serial routines loop until they reach a zero byte. This assumes the data is a null-terminated string.

A string in a hosted VNT program is managed by the runtime. A freestanding kernel cannot assume the hosted runtime exists. The compiler's freestanding subset has specific limitations, so do not assume every ordinary VNT library feature is available in `os/kernel.vnt`.

## 5. Unsafe blocks and inline assembly

The kernel includes blocks such as:

```vnt
unsafe {
    asm volatile("...")
}
```

The `unsafe` marker indicates low-level operations that the compiler cannot make safe in the same way as ordinary checked code. Inline assembly embeds CPU instructions into generated code. A mistake can corrupt registers, memory, or CPU state.

The current kernel uses inline assembly for serial port I/O, CPU control, and linker-symbol addresses. Some expressions rely on compiler-generated stack layout, such as a hard-coded offset from `%rbp`. That is fragile and is documented as a current implementation limitation—not recommended as a general coding technique.

## 6. Modules: the kernel is no longer one VNT file

The kernel entry file imports its subsystems explicitly:

```vnt
import "serial.vnt"
import "console.vnt"
import "ps2.vnt"
import "interrupts.vnt"
```

Each path is relative to `os/kernel.vnt`. The module loader combines the imported declarations for the freestanding compile; these imports do not create separate processes or ELF files. The assembly boundary remains separate because the CPU interrupt frame and `iretq` return sequence need a precisely defined ABI.

When you move a function into another module, keep its name and call contract stable, then rebuild with `bash os/build.sh`. Do not assume the full hosted standard library is available to imported kernel code.

## 7. Read, don't guess

Open `os/kernel.vnt`. Find:
- the declaration of `kernel_main`;
- the `while` loop that reads characters;
- the `if` branch for `help`;
- the function `serial_puts`.

For each, explain the purpose in plain language before trying to change it.

## Checkpoint

Explain the difference between ordinary VNT code and the kernel's freestanding VNT code. In particular, why can't the kernel automatically use every hosted runtime function?

## Exercise

Write a tiny VNT function that takes two integers and returns their sum. Compile and run it as an ordinary hosted program using the repository's documented VNT build process. Do not put this test in the kernel yet.