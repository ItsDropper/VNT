# Lesson 24 — Build and Verify the Interrupt Kernel

**Goal:** prove that the multi-module VNTOS kernel builds, boots, and receives
timer interrupts in QEMU, while understanding the boundary between x86-64
mechanics and VNT kernel policy.

## 1. Source ownership

| File | Responsibility |
| --- | --- |
| `os/arch/x86_64/boot.S` | Multiboot2 header, bootstrap page tables, long-mode transition, bootstrap stack |
| `os/arch/x86_64/interrupts.S` | IDT gates, register save/restore, PIC remapping, PIT programming, `iretq` |
| `os/interrupts.vnt` | VNT-side interrupt dispatch and exception policy |
| `os/kernel.vnt` | Kernel entry, shared state helpers, shell policy |
| `os/build.sh` | Compiler, assembler, linker, and optional ISO pipeline |

The build script assembles the two architecture files as separate objects and
links them with the compiler-generated VNT kernel object. The linker script
continues to define the ELF layout and entry symbol. Moving the files does not
change their machine instructions or ABI.

## 2. Build from the repository root

Run:

```sh
bash os/build.sh
```

Then inspect the generated artifact:

```sh
grub-file --is-x86-multiboot2 build/kernel.elf
readelf -h build/kernel.elf
nm -u build/kernel.elf
```

The first command should exit successfully. The ELF header should identify an
ELF64 x86-64 executable. `nm -u` should print no unresolved symbols for this
freestanding image. These checks cannot prove the CPU executes the code
correctly. The ISO is only created when the GRUB and xorriso tools are present.

## 3. Trace IRQ0 from hardware to policy

1. The PIT raises IRQ0 and the remapped PIC presents vector 32.
2. The x86-64 entry stub normalizes the vector/error-code stack shape and saves
   general-purpose registers.
3. Assembly calls `interrupt_dispatch`, the VNT policy function.
4. VNT increments the timer counter and returns without logging or allocating
   inside interrupt context.
5. Assembly acknowledges the PIC, restores registers, discards the normalized
   vector/error pair, and executes `iretq`.
6. The shell's `irq` command reads the counter outside interrupt context.

The counter proves timer delivery when it changes; it is not a scheduler.
Keyboard input remains polled and IRQ1 stays masked.

## 4. Boot in QEMU

For a serial-only session:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Wait for `VNTOS_COMMAND_LINE_READY`. Type these commands one at a time:

```text
help
about
irq
echo test
mem
mouse
```

Leave the shell idle for a few seconds, run `irq` again, and check that the
number increased. Try an unknown command to verify normal shell behavior.

## 5. What CI actually proves

The `VNTOS Boot and Serial Shell` GitHub Actions workflow builds the kernel,
checks its Multiboot2/ELF properties and unresolved symbols, boots it in QEMU,
sends commands through the emulated keyboard monitor, validates serial output,
and checks the VGA text buffer. Inspect the workflow run for the exact branch
or pull request and confirm the job passed; a compiler build alone does not
prove that the kernel boots.

## 6. Current limitations

This refactor establishes a source/build boundary, not a complete architecture
abstraction. The bootstrap still defines `vntos_state`; VNT code accesses it
through inline assembly. Several low-level functions depend on
compiler-generated stack offsets, and imported VNT modules are flattened into
one compilation unit. Do not infer memory-safe ownership, a scheduler, or
user/kernel isolation from this directory structure.

## Debug by layer

- **VNT compile fails:** inspect the first parser/type error and import path.
- **Assembler fails:** inspect the first reported line in the relevant file
  under `os/arch/x86_64/`.
- **Link fails:** verify the three object files are passed to `ld` and that
  `interrupt_dispatch` resolves.
- **Kernel boots but no prompt:** inspect the serial transcript and early debug
  output.
- **Prompt works but ticks stay zero:** inspect IDT initialization, PIC masks,
  PIT divisor programming, vector 32 stub, and EOI path in that order.

## Checkpoint

Explain what each of `grub-file`, `readelf`, `nm`, and QEMU proves—and what
it cannot prove. Record the tested commit SHA and actual CI result; never reuse
an old successful transcript as evidence for a new revision.
