# Lesson 15 — Debugging Boot Failures

**Goal:** use evidence to narrow down where startup failed.

## 1. Start with the earliest error

If `bash os/build.sh` fails, find the first error in the output. Fixing a later error before the first one often wastes time because later failures may be consequences.

If the build succeeds but QEMU does not show the shell prompt, the failure is at runtime rather than necessarily in the compiler.

## 2. Use a layered diagnosis

### Layer A: Did the build create the expected files?

Check:

```bash
ls -l build/kernel.elf build/vnt
```

If the ISO is expected, check it too:

```bash
ls -l build/vntos.iso
```

If the ELF exists but the ISO does not, review the build script's final message and verify that the GRUB/ISO tools are installed.

### Layer B: Is the ELF structurally plausible?

```bash
file build/kernel.elf
readelf -h build/kernel.elf
readelf -l build/kernel.elf
```

Confirm it is an x86-64 ELF and inspect the entry point and loadable segments. If available, use the build script's Multiboot2 validation.

### Layer C: Does GRUB load it?

Check the GRUB path inside the ISO and confirm it matches `os/grub.cfg`. The configuration expects `/boot/kernel.elf` inside the image.

### Layer D: How far did the bootstrap get?

The assembly writes marker bytes to QEMU's debug port `0xE9`. Those markers can help distinguish early bootstrap stages when QEMU is configured to expose that port. They are not COM1 output, so they will not necessarily appear in the serial terminal.

### Layer E: Did the serial console start?

If the kernel prints its startup banner but no prompt, inspect serial initialization and the shell setup. If the prompt appears but input is stuck, inspect the receive loop. If characters echo but commands fail, inspect line termination and command comparisons.

## 3. Use QEMU consistently

The documented launch command is:

```bash
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Keep the launch configuration unchanged while debugging one variable at a time. Stop the VM with `Ctrl+C` in the host terminal.

## 4. Do not guess at memory failures

A wrong page-table entry, stack pointer, or linker address can cause a very early failure. Do not randomly change multiple control-register bits. Compare the code to the intended sequence, inspect the generated assembly and ELF metadata, and change only one hypothesis at a time.

## 5. Write a minimal failure report

When asking for help, include:
- the exact build command;
- the first error message;
- whether `build/kernel.elf` and `build/vntos.iso` exist;
- the exact QEMU command;
- the last output you observed;
- the commit or source revision you tested.

Do not claim “it doesn't work” without the observed stage. That information is what makes the problem diagnosable.

## Checkpoint

For each symptom—build failure, no ISO, no startup output, startup output but no prompt, prompt but no input—name the first file or layer you would inspect.