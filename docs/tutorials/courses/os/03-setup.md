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

## 7. Keep build artifacts and source separate

The build script writes generated files under `build/`. Treat those as outputs. If you need to inspect generated assembly, open it to understand the compiler's output, but make a permanent fix in `os/*.vnt`, `os/*.S`, the linker script, or the compiler source as appropriate. Editing a generated file is not reproducible because the next build overwrites it.

Check the repository status before and after building:

```sh
git status --short
bash os/build.sh
git status --short
```

Know which generated artifacts are ignored and which are tracked. Do not add a large ISO or object file to a source commit unless the repository explicitly expects that artifact.

## 8. Use a consistent QEMU command

Start with the documented QEMU command in `os/README.md`. Keep the memory size, boot medium, display mode, and serial routing fixed while debugging. If you change several settings between runs, you lose the ability to compare results.

The graphical QEMU window shows VGA text output and accepts emulated PS/2 keyboard input. The serial console is a separate channel. For a serial-only run, `-display none -serial stdio` hides VGA output; it does not mean VGA initialization failed. Always record which output channel you are watching.

## 9. Tool failures are not kernel failures

If `gcc`, `nasm`, `grub-file`, `xorriso`, or QEMU is missing, the first failure may be an environment problem. Confirm the executable exists and note its version before changing kernel source. If the compiler builds but the linker reports an undefined symbol, that is a different class of problem: inspect the symbol name and whether its object file was included.

Do not install random packages until you know which stage needs the missing tool. The build pipeline in Lesson 05 explains which tool is used at each stage.

## 10. A safe repeatable lab loop

Use this sequence for every later lesson:

1. Check `git status --short` and make sure unrelated work is not mixed in.
2. Read the relevant source and identify the exact function you intend to change.
3. Make one focused edit.
4. Run `bash os/build.sh` and preserve the first failure.
5. Boot the resulting ISO in QEMU.
6. Exercise the specific behavior changed and one existing behavior.
7. Inspect `git diff --check` and `git diff`.
8. Record the result and revert experiments that you cannot explain.

## Lab deliverable

Write down the exact commands and tool versions used for a successful baseline. Include whether the ISO was created, whether QEMU booted, whether the VGA prompt appeared, and whether the serial shell responded. This turns "it works on my machine" into a reproducible result.

## 11. Preserve the baseline

Before experimenting, make sure the working tree contains only changes you understand. If you are working on an existing project, do not reset or clean unrelated files just to make a build look tidy. Build artifacts are disposable; source and personal data are not.

When a command fails, keep the complete first error and the exact command. If the problem is a missing package, record the package name and the environment. If the problem is a source error, record the file and line. This prevents an environment problem from being misdiagnosed as a kernel regression.

## Lab: make your setup reproducible

Write a short environment record containing operating system or WSL version, GCC version, assembler version, GRUB/ISO tools availability, QEMU version, repository commit, and exact build command. Add one sentence explaining which stages are skipped when ISO packaging tools are absent. This record should be enough for another developer to reproduce your baseline.
