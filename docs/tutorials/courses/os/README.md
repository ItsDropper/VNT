# Build VNTOS: A Beginner's Operating-System Course

**Goal:** understand, build, boot, inspect, and safely extend the real learning kernel in this repository's `os/` directory.

This is a guided course, not a claim that VNTOS is a finished desktop operating system. The current project is an experimental x86-64 kernel with a Multiboot2 bootstrap, early identity mapping, VNT-compiled kernel entry, and a serial command shell. It does **not** yet have a general memory allocator, filesystem, process scheduler, user mode, or a general device-driver framework.

## Who this course is for

You do not need prior operating-system development experience. You should be willing to read small programs and use a terminal. If programming is new to you, take the beginner lessons in [VNT Programming Courses](../../README.md) alongside this course. Every lesson explains new terms before relying on them.

You will learn what the computer does before the kernel starts, why a bootloader is used, what the CPU bootstrap must configure, how the kernel is linked, how VNT code runs without a hosted runtime, how serial input/output works, and how to test changes in an emulator.

## Safety and scope

- Build and test in **QEMU**, not on your main computer's physical disk.
- Do not write kernel images to USB drives, internal disks, or other storage containing important data.
- Run build commands from the repository root unless a lesson explicitly says otherwise.
- The source is the final authority. If a lesson and the current code disagree, note the mismatch and inspect the current source before changing anything.
- A successful compile does not prove the kernel boots. Use the boot and shell checks later in this course.
- Some low-level parts are experimental and compiler-specific. The course calls those limitations out instead of presenting them as ideal production design.

## How to use each lesson

1. Read the goal and vocabulary.
2. Follow the source-reading task before editing code.
3. Run the stated command and compare your result with the expected outcome.
4. Complete the checkpoint in your own words.
5. Make one small change at a time, then rebuild and test in QEMU.

Do not skip ahead to paging or CPU mode switching before understanding the boot path.

## Course map

### Phase 1 — Foundations and setup
1. [Start here: your first kernel lab](00-start-here.md)
2. [What an operating system does](01-what-is-an-os.md)
3. [The machine underneath the OS](02-computer-basics.md)
4. [Set up the safe development environment](03-setup.md)
5. [Learn just enough VNT for kernel code](04-vnt-for-kernels.md)

### Phase 2 — From source code to a bootable ELF
6. [Follow the build pipeline](05-build-pipeline.md)
7. [Understand the boot sequence](06-the-boot-sequence.md)
8. [Understand GRUB and Multiboot2](07-grub-and-multiboot2.md)
9. [Read the assembly bootstrap](08-reading-boot-assembly.md)
10. [Enter x86-64 long mode](09-long-mode.md)
11. [Understand the initial page tables](10-initial-paging.md)
12. [Understand ELF and the linker script](11-elf-and-linker.md)

### Phase 3 — The kernel written in VNT
13. [Hosted programs versus freestanding kernels](12-freestanding-vnt.md)
14. [Serial output and input](13-serial-io.md)
15. [Read the command shell](14-the-serial-shell.md)

### Phase 4 — Work like a kernel developer
16. [Debugging boot failures](15-debugging.md)
17. [Test changes without guessing](16-testing.md)
18. [Add a shell command as your first modification](17-first-kernel-change.md)
19. [Where to go next: real OS subsystems](18-next-steps.md)

## What counts as finishing?

You have completed the course when you can explain the path from GRUB to `kernel_main`, build and boot the current kernel in QEMU, describe the purpose and limits of the initial 1 GiB identity map, trace serial input through the shell, add a harmless command in VNT, and prove that your change still builds and boots.

## Repository source map

| File | Responsibility |
| --- | --- |
| `os/boot.S` | Multiboot2 header and early CPU bootstrap |
| `os/kernel.vnt` | Kernel entry, serial routines, command parsing and shell |
| `os/linker.ld` | ELF layout, load addresses and linker symbols |
| `os/grub.cfg` | Tells GRUB to load the kernel using Multiboot2 |
| `os/build.sh` | Builds the compiler, kernel object, ELF and optional ISO |
| `LICENSE` | Project license |

Start with [Lesson 00](00-start-here.md).