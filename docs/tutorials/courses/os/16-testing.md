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

## 7. Turn each invariant into a test

A test is useful when it would fail if a requirement were violated. "The kernel boots" is a valuable smoke test, but it will not catch every shell or timer bug. For a command buffer, test the empty line and the maximum supported length. For the timer, read the count twice after a delay. For the build pipeline, check the ELF and Multiboot2 header before booting the ISO.

Write the expected result before running the test. Otherwise it is easy to reinterpret unexpected output as success after the fact. Preserve the actual output, including exit codes and the first failing assertion.

## 8. Separate regression tests from experiments

A regression test protects behavior that already works. An experiment helps you learn about a new or uncertain behavior. Keep them distinct: do not make the default CI intentionally crash the kernel just to demonstrate exception handling. A controlled fault experiment belongs in a separate QEMU run with a known reset path and a clear expected result.

The existing VNTOS workflow exercises boot, shell input, and VGA output. The `irq` command adds a way to inspect timer state. If the workflow only checks that the word `irq` appears, it does not prove that the numeric count increased; describe exactly what the assertion checks.

## 9. Test the negative path

Every feature has a boundary where it should reject or safely handle input. An unknown shell command should not invoke an unrelated operation. A too-long line must not overwrite memory. An unexpected CPU exception should not be silently treated as a successful return. A failed build must stop the pipeline rather than package stale objects.

Negative-path testing is especially important in kernels because a bad fallback can turn a clear error into silent memory corruption. Choose one invalid input and one failure condition for each new feature before calling it complete.

## Lab: write a test plan before implementation

For one proposed kernel change, define a happy-path test, a boundary test, a failure test, and a regression test. Name the command or observation for each. Then implement only the feature and tests that can be verified with the current QEMU setup.

## 10. Test the build script itself

The build script is executable project logic and can regress independently of the kernel. It must stop when compilation or linking fails, avoid packaging stale artifacts after a failure, and clearly distinguish required steps from optional ISO tooling. Read `os/build.sh` and identify where `set -euo pipefail` affects failure propagation.

When reviewing a build-script change, reason about what happens if each command exits nonzero. If a later command can still run after an earlier failure, the script may accidentally report a misleading success. Keep the test focused on the script's actual behavior and do not claim coverage that CI does not provide.
