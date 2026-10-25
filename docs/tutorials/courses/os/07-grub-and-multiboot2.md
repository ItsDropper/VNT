# Lesson 07 — GRUB and Multiboot2

**Goal:** understand why the kernel includes a Multiboot2 header and how GRUB uses it.

## 1. The bootloader's job

The bootloader locates a kernel image, loads its loadable portions into memory, and transfers control according to a defined protocol. This keeps the early kernel from having to implement every detail of reading boot media before it can run.

VNTOS uses GRUB and the Multiboot2 protocol. GRUB's configuration is in `os/grub.cfg`.

## 2. Read the GRUB configuration

The file contains a menu entry similar to:

```grub
set timeout=0
set default=0

menuentry "VNTOS - VNT Command Shell" {
    multiboot2 /boot/kernel.elf
    boot
}
```

The `multiboot2` command tells GRUB to load the named kernel using the Multiboot2 protocol. The path is inside the ISO's filesystem, not a path on your host operating system. The build script copies the kernel to `build/iso/boot/kernel.elf` and the configuration to `build/iso/boot/grub/grub.cfg` before making the ISO.

## 3. The header in `boot.S`

The beginning of `os/boot.S` defines a section named `.multiboot`. It includes a magic value, architecture field, header length, checksum, and an end tag. These fields allow a compliant bootloader to recognize the image as a Multiboot2 kernel.

The header must be placed where the bootloader can find it in the kernel file, and its values must satisfy the protocol. This is one reason the linker script preserves the section using `KEEP(*(.multiboot))`.

## 4. Validate the artifact

The build script optionally runs:

```bash
grub-file --is-x86-multiboot2 build/kernel.elf
```

A successful exit means the installed tool recognizes the image as a Multiboot2 kernel. It does not prove that the CPU bootstrap or shell works.

If the command fails, inspect whether the header is present, correctly formed, and retained near the beginning of the ELF image. Check the build output and the linker script rather than changing random header constants.

## 5. Boot protocol versus CPU setup

Multiboot2 describes the bootloader-to-kernel handoff. It does not automatically configure every CPU feature your kernel wants. VNTOS still uses its own assembly to construct initial page tables, enable long mode, and establish the 64-bit stack.

Do not conflate “GRUB loaded the kernel” with “the kernel has completed its own initialization.”

## Checkpoint

Explain the difference between `os/grub.cfg`, the Multiboot2 header in `os/boot.S`, and `os/linker.ld`.

## Exercise

Find the four header fields listed in Section 3 in `os/boot.S`. Then find the `KEEP` directive in the linker script and explain why the kernel needs that section to survive linking.