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

## 7. Inspect each stage independently

A build pipeline is easier to debug when every stage has a check. After the build, list the artifacts rather than assuming that the script reached the final step:

```sh
ls -l build/
file build/kernel.elf build/kernel.o build/boot.o build/interrupts.o
```

If a file is absent, identify the command that should have produced it. If an object exists but the ELF does not, the failure is likely in linking. If the ELF exists but the ISO does not, inspect the GRUB/ISO packaging stage. Use the actual filenames from the current build script because generated artifact names can change.

## 8. Understand what the linker resolves

The assembler can accept a call to a symbol whose definition is in another object file. The linker later has to find that definition. VNT's freestanding backend emits function symbols using its own naming convention, so the source-level name and the ELF symbol may differ. The interrupt assembly must call the emitted symbol name, not a name guessed from the VNT source.

Inspect symbols with:

```sh
nm build/kernel.elf
readelf -s build/kernel.elf
```

Use these tools to answer a concrete question, such as whether the interrupt dispatcher exists in the final ELF. A symbol table entry does not prove the function executes correctly; it only proves the symbol is present in that artifact.

## 9. Why the custom linker script matters

The linker script controls the layout and alignment of sections and defines symbols that the bootstrap or kernel may use. Changing the start address can invalidate assumptions made by the bootloader header, initial page tables, or assembly code. Treat an address change as an architectural change, not a cosmetic cleanup.

When editing `os/linker.ld`, identify which input sections are collected into each output section, where the image begins, and how the script exposes boundaries. Check the resulting ELF with `readelf -h -l` and compare its entry point and load segments with the previous build.

## 10. The ISO is a container, not the kernel

The ELF is the linked kernel image. The ISO contains bootloader configuration and the kernel in a layout GRUB can read. A valid ISO can still contain a bad ELF; a valid ELF can still be packaged incorrectly. Therefore test both artifacts separately and then boot the ISO.

The build driver VBuild does not currently replace this mixed VNT/assembly/custom-linker/GRUB pipeline. Use `bash os/build.sh` as the authoritative kernel build, and use `docs/VBUILD.md` for the supported scope of VBuild.

## Lab: make a stage table

Create a table with columns for command, input, output, and validation. Include the VNT compiler build, VNT freestanding emission, assembly of `boot.S`, assembly of `interrupts.S`, linking, Multiboot2 validation, and ISO creation. Fill in the actual paths from `os/build.sh`, not guessed names.

## 11. Make the build explain itself

For every generated artifact, record the command that creates it and the next command that consumes it. This forms a directed graph: VNT sources feed the freestanding compiler; generated assembly feeds the assembler; object files feed the linker; the ELF feeds validation and GRUB packaging; the ISO feeds QEMU.

If a future build-script edit changes a filename, update the next stage and the course together. A pipeline diagram that no longer matches `os/build.sh` is worse than no diagram because it sends debugging toward nonexistent files.
