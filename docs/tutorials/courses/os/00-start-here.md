# Lesson 00 — Start Here: Your First Kernel Lab

**Time:** about 20–40 minutes  
**You need:** this repository, a terminal, and curiosity  
**You will finish with:** a clear map of the course and a safe test plan

## 1. What are we building?

VNTOS is a small experimental kernel written primarily in VNT. A small assembly file prepares the processor so compiled kernel code can run. GRUB loads the kernel, and QEMU lets you run it as a virtual machine instead of booting your real computer from it.

The current kernel is intentionally small. It can print to and read from a serial console and interpret a few commands. It is a learning platform—not a replacement for Windows or Linux.

## 2. Learn the words

- **Source code:** text that describes a program, such as `os/kernel.vnt`.
- **Compiler:** translates source code into a lower-level representation or machine instructions.
- **Object file:** compiled code and data that has not necessarily been linked into the final program.
- **Linker:** combines object files and arranges their addresses.
- **Kernel:** the privileged core that manages the computer's resources.
- **Bootloader:** software that loads a kernel and transfers control to it.
- **Emulator:** software that models a computer so the guest system can run in a controlled environment.
- **Serial console:** a simple byte stream often used for debugging systems before graphics are available.

## 3. Open the important files

From the repository root, inspect these files in your editor:

1. `os/README.md`
2. `os/build.sh`
3. `os/grub.cfg`
4. `os/linker.ld`
5. `os/boot.S`
6. `os/kernel.vnt`

Do not try to understand every instruction yet. Just identify which file is responsible for building, boot configuration, memory layout, processor startup, and shell behavior.

## 4. Draw the system

Write down this initial model:

```text
Computer / QEMU
    ↓
GRUB loads the kernel ELF
    ↓
os/boot.S prepares the CPU
    ↓
generated VNT entry point
    ↓
kernel_main() in os/kernel.vnt
    ↓
serial command shell
```

Later lessons will refine this diagram. It is more useful to have a rough correct model than to memorize isolated instructions.

## 5. Your safety rule

Use QEMU for experiments. Do not install the kernel on your physical computer, repartition a disk, or overwrite a USB drive for this course. The build creates files under the repository's `build/` directory. Keep valuable files backed up and do not delete files outside the project to solve a build problem.

## Checkpoint

Without looking back, answer:

1. What loads the kernel?
2. Which file prepares the CPU?
3. Which file contains the shell?
4. Why do we use QEMU?

**Answers to check:** GRUB loads the kernel; `os/boot.S` performs the early bootstrap; `os/kernel.vnt` contains the shell; QEMU lets us test without booting the kernel directly on physical hardware.

## Exercise

Write one sentence for each of the six files listed above describing what you think it does. Revisit your answers after Lesson 05.