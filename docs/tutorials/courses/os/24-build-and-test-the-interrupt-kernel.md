# Lesson 24 — Build and Verify the Interrupt Kernel

**Goal:** prove that the multi-module VNTOS kernel builds, boots, and receives timer interrupts in QEMU.

## 1. Build from the repository root

Run:

```sh
bash os/build.sh
```

The script rebuilds the VNT compiler, follows the imports from `os/kernel.vnt`, emits freestanding assembly, assembles `os/boot.S` and `os/interrupts.S`, then links all object files into `build/kernel.elf`. If GRUB and ISO utilities are installed, it also creates `build/vntos.iso`.

Do not assemble `os/interrupts.S` into the VNT-generated kernel object. It is a separate object with its own CPU entry symbols. The final linker command must include `build/boot.o`, `build/kernel.o`, and `build/interrupts.o`.

## 2. Check the artifact before booting

Run:

```sh
grub-file --is-x86-multiboot2 build/kernel.elf
readelf -h build/kernel.elf
nm -u build/kernel.elf
```

The first command should exit successfully. The ELF header should identify an ELF64 x86-64 executable. `nm -u` should print no unresolved symbols for this freestanding image. These checks are useful, but they cannot prove the CPU can execute the code correctly.

## 3. Boot in QEMU

For a serial-only session:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Wait for `VNTOS_COMMAND_LINE_READY`. Type the following commands one at a time:

```text
help
about
irq
echo test
mem
mouse
```

The help output should include `irq`. The command prints `Timer ticks: 0x...`. Leave the shell idle for a few seconds, run `irq` again, and check that the number increased. Then try an unknown command to ensure ordinary shell behavior still works.

If the ISO was not created, install the boot-image tools listed in `os/README.md`; do not attempt to boot a missing artifact.

## 4. Understand the automated check

The `VNTOS Boot and Serial Shell` GitHub Actions workflow builds the kernel, checks its Multiboot2/ELF properties, boots it in QEMU, sends commands through the emulated keyboard monitor, and validates serial output. This change extends that check to request `irq` and require the timer counter label.

A green build alone is not the same as a passing QEMU test. Inspect the Actions run for the branch or pull request and confirm the boot-and-shell job completed. Do not claim the interrupt implementation is verified until that run actually passes.

## 5. Debug by layer

- **VNT compile fails:** inspect the first VNT parser/type error and the imported module path.
- **Assembler fails:** inspect the first reported line in `os/interrupts.S`; do not guess at macro expansion.
- **Link fails:** check that all three object files are passed to `ld` and that `interrupt_dispatch` resolves.
- **Kernel boots but no prompt:** inspect the serial transcript and early debug output.
- **Prompt works but ticks stay zero:** check the IDT initialization call, PIC masks, PIT divisor programming, vector 32 stub, and EOI path in that order.

## Checkpoint

Explain what each of `grub-file`, `readelf`, `nm`, and QEMU proves—and what it cannot prove.

## Exercise

Capture the output of two `irq` commands separated by a delay. Keep the exact command transcript with your notes. If the values do not change, report the observed behavior instead of changing multiple subsystems at once.
