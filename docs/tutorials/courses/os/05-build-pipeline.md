# Lesson 05 — Follow the Build Pipeline

**Goal:** understand how source files become the bootable kernel ELF and ISO.

## 1. One source file is not the whole kernel

The kernel is made from more than `kernel.vnt`. The assembly bootstrap supplies the Multiboot2 header and early CPU setup. The VNT compiler produces kernel code. The linker combines compiled objects and assigns addresses. GRUB configuration tells the bootloader how to load the resulting ELF.

## 2. Read `os/build.sh`

The script runs these stages in order:

1. Create the `build/` directory.
2. Compile the VNT compiler itself from C source files into `build/vnt`.
3. Ask that compiler to emit freestanding assembly from `os/kernel.vnt`; its imports load `serial.vnt`, `console.vnt`, `ps2.vnt`, and `interrupts.vnt` into the same compilation unit.
4. Assemble the generated VNT assembly into `build/kernel.o`.
5. Assemble `os/boot.S` into `build/boot.o` and `os/interrupts.S` into `build/interrupts.o`.
6. Link all three object files with `os/linker.ld` into `build/kernel.elf`.
7. If `grub-file` is installed, verify that the ELF has a Multiboot2 header.
8. If the ISO tools are installed, package the ELF and GRUB configuration into `build/vntos.iso`.

Each output becomes an input to the next stage. If the compiler fails, later stages cannot produce a valid new kernel.

## 3. Source, assembly, object, ELF, ISO

- `.vnt`: source code for the VNT compiler.
- `.s`: assembly text emitted by the compiler.
- `.o`: object file produced by the assembler.
- `.elf`: linked executable image with sections and load information.
- `.iso`: disk-image format that can contain the kernel and bootloader files.

The ISO is a container used by the virtual machine's boot process; it is not the same thing as the kernel ELF.

## 4. Rebuild after edits

From the repository root:

```bash
bash os/build.sh
```

The script uses `set -euo pipefail`, so a failing command stops the build rather than silently continuing. Read the earliest error carefully. A later linker error may simply mean an earlier stage did not create an expected file.

## 5. Inspect generated files

After a successful build:

```bash
file build/kernel.elf
file build/boot.o
file build/kernel.o
file build/interrupts.o
```

If available, inspect ELF metadata:

```bash
readelf -h build/kernel.elf
readelf -l build/kernel.elf
```

The header describes the ELF format and architecture. The program headers describe loadable segments. You do not need to memorize every field yet.

## 6. Why the build can succeed but boot fail

Compilation checks that source can be translated. Linking checks that object files can be combined according to the linker rules. Neither guarantees the bootloader accepts the image, that the CPU setup is correct, or that the kernel's input loop behaves properly.

That is why later lessons use both artifact inspection and QEMU runtime checks.

## Checkpoint

Draw the build pipeline from the imported VNT modules, `boot.S`, and `interrupts.S` to `kernel.elf`, then from the ELF to the ISO. Label which tool handles each transformation.

## Exercise

Temporarily inspect the build script and find the exact command that links `build/boot.o`, `build/kernel.o`, and `build/interrupts.o`. Do not edit the script in this lesson.