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

## 6. Understand the Multiboot2 header fields

The header has a magic value, an architecture field, a header length, and a checksum chosen so the protocol's required sum is zero modulo (2^{32}). It also contains aligned tags that request or describe features. The bootloader searches for this structure within the range and alignment required by the protocol; putting a similar-looking byte sequence in an arbitrary section is not enough.

When inspecting the header, verify its placement and checksum from the actual assembly and use `grub-file --is-x86-multiboot2 build/kernel.elf` as an independent validation step. The tool's success says the ELF looks like a Multiboot2 kernel; it does not say the bootstrap's paging code is correct.

## 7. ELF loading and ISO packaging are separate

GRUB reads the ISO filesystem, finds `grub.cfg`, loads the kernel ELF, and interprets its loadable segments. The kernel does not run directly from the ISO file. This distinction matters when debugging: a missing GRUB configuration is a packaging problem, while a wrong ELF entry point is a kernel-image problem.

Use the following checks after building:

```sh
file build/vntos.iso build/kernel.elf
readelf -h -l build/kernel.elf
grub-file --is-x86-multiboot2 build/kernel.elf
```

Run only commands for artifacts that actually exist. If the ISO tools were unavailable, say so explicitly rather than treating the missing ISO as a boot failure.

## 8. Why boot protocol code should be stable

The Multiboot2 header is a contract with GRUB. Changing its alignment, tags, or section placement can prevent the loader from recognizing the kernel before any of your normal debugging output can execute. Keep protocol changes isolated and validate the ELF after each one.

Do not copy a random boot header from another project without checking whether it targets the same architecture, protocol version, and linker layout. A header written for 32-bit entry assumptions may not match this kernel's 64-bit transition.

## Lab: separate the layers

Create three failure examples: (a) the ISO contains no `grub.cfg`; (b) the ELF fails Multiboot2 validation; (c) the ELF validates but QEMU resets immediately. For each, write the first tool or source file you would inspect and explain why. This trains you to diagnose the earliest failing contract rather than randomly editing the boot code.

## 9. Bootloader responsibility ends at the handoff

GRUB loads the kernel according to the boot protocol and transfers control. It does not initialize VNTOS's UART driver, parse shell commands, create a heap, or decide what an IRQ means. Those responsibilities belong to the kernel after the handoff. Keeping that division clear helps you avoid debugging GRUB when the failure is actually in kernel initialization.

A bootloader can also provide information through protocol-defined structures or registers. If VNTOS later consumes a memory map or framebuffer description, parse the documented structure and validate its size and fields before using it. Never treat a pointer supplied by the bootloader as automatically valid forever.

## Lab: verify the handoff without assumptions

Write down what the boot protocol guarantees and what the bootstrap itself must establish. For each item, point to the relevant header, assembly block, linker setting, or kernel initializer. Mark any behavior that has not been independently tested. This creates a concrete contract for future changes to the boot path.
