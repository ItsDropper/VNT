# Lesson 03 — Set Up a Safe Development Environment

**Goal:** prepare a Linux or WSL environment that can compile the kernel and run it in QEMU.

## 1. Why Linux or WSL?

The current build script uses GCC, GNU binutils, and optionally GRUB and ISO creation tools. The simplest documented path is Debian, Ubuntu, or WSL with an Ubuntu/Debian environment. You do not need to replace your computer's operating system.

## 2. Install the tools

In a Debian/Ubuntu/WSL terminal, run:

```bash
sudo apt-get update
sudo apt-get install --yes build-essential binutils grub-pc-bin grub-common xorriso mtools qemu-system-x86
```

What these packages provide:
- `build-essential`: compiler and standard build utilities;
- `binutils`: assembler and linker tools;
- `grub-pc-bin` and `grub-common`: GRUB tooling for a bootable ISO;
- `xorriso`: ISO image creation support;
- `mtools`: tools used by some boot-image workflows;
- `qemu-system-x86`: the virtual x86 machine used for testing.

If you are using a different Linux distribution, package names may differ.

## 3. Confirm the tools

Run:

```bash
gcc --version
ld --version
qemu-system-x86_64 --version
```

If any command is not found, check the installation output before proceeding. Do not randomly download executables from unofficial mirrors.

## 4. Locate the repository root

The repository root is the directory containing `os/`, `src/`, and the top-level README. Change into that directory before running the build:

```bash
cd /path/to/VNT
pwd
ls
```

Replace `/path/to/VNT` with the real location. The final command should show directories such as `os` and `src`.

## 5. Build the kernel

Run:

```bash
bash os/build.sh
```

The script builds the VNT compiler from the listed C source files, compiles `os/kernel.vnt` to freestanding assembly, assembles that output and `os/boot.S`, then links the kernel ELF. If the GRUB and ISO tools are present, it also creates `build/vntos.iso`.

Expected artifacts:
- `build/kernel.elf`: linked kernel;
- `build/vntos.iso`: bootable ISO, when ISO tooling is available.

A missing ISO does not necessarily mean the kernel compile failed. Read the final build-script messages.

## 6. Boot in QEMU

If the ISO exists, run:

```bash
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

You should see the VNTOS startup text and a `VNTOS>` prompt. Type `help` and press Enter. Stop QEMU with `Ctrl+C` in the terminal.

## Safety checklist

- Use the emulator, not physical-disk installation.
- Do not run destructive disk or partitioning commands for this course.
- Keep edits inside the repository.
- If a command fails, save the first error message; it is often more useful than the later cascade of errors.

## Checkpoint

Explain what `build/kernel.elf` is and why `build/vntos.iso` is a different artifact.