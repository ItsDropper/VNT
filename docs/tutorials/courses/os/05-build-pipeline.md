# Lesson 05 — Follow the Build Pipeline

**Goal:** understand how source files become the bootable kernel ELF and ISO.

## 1. One source file is not the whole kernel

The kernel is made from more than `kernel.vnt`. VNT imports provide the kernel's source-level modules. The x86-64 bootstrap establishes the CPU state needed to enter compiled code. The interrupt assembly provides the CPU-defined entry/exit boundary. The linker combines objects and assigns addresses; GRUB configuration tells the bootloader how to load the resulting ELF.

## 2. Read `os/build.sh`

The script runs these stages in order:

1. Create the `build/` directory.
2. Compile the VNT compiler itself from C source files into `build/vnt`.
3. Ask the compiler to emit freestanding assembly from `os/kernel.vnt`; its imports load `serial.vnt`, `console.vnt`, `ps2.vnt`, and `interrupts.vnt` into the same compilation unit.
4. Assemble the generated VNT assembly into `build/kernel.o`.
5. Assemble `os/arch/x86_64/boot.S` into `build/boot.o` and `os/arch/x86_64/interrupts.S` into `build/interrupts.o`.
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

```sh
file build/kernel.elf build/boot.o build/kernel.o build/interrupts.o
readelf -h build/kernel.elf
readelf -l build/kernel.elf
nm -u build/kernel.elf
```

The header describes the ELF format and architecture. The program headers describe loadable segments. `nm -u` should show no unresolved symbols in the final freestanding image.

## 6. Why the build can succeed but boot fail

Compilation checks that source can be translated. Linking checks that object files can be combined according to the linker rules. Neither guarantees the bootloader accepts the image, that CPU setup is correct, or that the input loop behaves properly. Later lessons use artifact inspection and QEMU runtime checks.

## Checkpoint

Draw the pipeline from the imported VNT modules, `os/arch/x86_64/boot.S`, and `os/arch/x86_64/interrupts.S` to `kernel.elf`, then from the ELF to the ISO. Label which tool handles each transformation.

## Exercise

Find the exact command that links `build/boot.o`, `build/kernel.o`, and `build/interrupts.o`. Do not edit the script in this lesson.

## 7. What the architecture boundary does—and does not—mean

The assembly files live under `os/arch/x86_64/` because they implement x86-64-specific boot and interrupt mechanics. VNT retains shell and interrupt policy. However, this is not yet a complete architecture-independent kernel interface: shared state is still defined in assembly and accessed through compiler-specific inline assembly, and imported VNT modules are flattened into one compilation unit.

The build driver VBuild does not currently replace this mixed VNT/assembly/custom-linker/GRUB pipeline. Use `bash os/build.sh` as the authoritative kernel build, and use `docs/VBUILD.md` for VBuild's supported scope.
