# Lesson 16 — Test Changes Without Guessing

**Goal:** learn a repeatable test routine for kernel changes.

## 1. A kernel needs more than a compile check

A compiler exit code of zero means the current source passed the compiler and linker stages. It does not prove the bootloader accepts the image, that the CPU reaches long mode, or that the shell reads input correctly.

A useful test plan covers several layers.

## 2. Build test

From the repository root:

```bash
bash os/build.sh
```

Confirm the command exits successfully and produces `build/kernel.elf`. If ISO tools are installed, confirm `build/vntos.iso` is produced. Save the first failure if the build fails.

## 3. Artifact test

Run:

```bash
file build/kernel.elf
readelf -h build/kernel.elf
readelf -l build/kernel.elf
```

Check that the ELF is the expected architecture and has plausible loadable segments. If `grub-file` is installed, the build script also checks Multiboot2 recognition.

## 4. Runtime smoke test

Boot the ISO using:

```bash
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Check that:
1. the startup banner appears;
2. the prompt appears;
3. `help` prints the command list;
4. `about` prints the description;
5. `echo test` prints `test`;
6. an unknown command prints an error;
7. `Ctrl+C` can stop QEMU from the host.

This is a smoke test, not a complete verification of the kernel.

## 5. Test boundaries and failure cases

For any change to command input, also consider:
- empty input;
- a command at the maximum supported length;
- backspace on an empty line;
- an unknown command;
- repeated commands;
- input that shares a prefix with a valid command.

For low-level memory or bootstrap changes, tests should include build validation, artifact checks, QEMU boot evidence, and any targeted tests that cover the changed behavior. Do not infer hardware correctness from a single QEMU success.

## 6. Report the evidence accurately

Separate these claims:
- **Built:** compilation and linking succeeded.
- **Booted:** the VM reached a defined runtime milestone.
- **Command tested:** a specific command produced the expected result.
- **CI passed:** a named workflow run completed successfully on the relevant commit.

Do not describe a test as passed if it was not run, and do not rely on a workflow from an older commit without noting the revision difference.

## Checkpoint

Write a test checklist for a hypothetical change to `echo`. Include both the expected result and at least one edge case.

## Exercise

Boot the unmodified kernel first and record the output for `help`, `about`, `echo test`, and an unknown command. This is your baseline for Lesson 17.