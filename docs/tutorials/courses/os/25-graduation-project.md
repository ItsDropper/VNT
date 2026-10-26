# Lesson 25 — Graduation Project: Make One Kernel Subsystem Better

**Goal:** finish by changing a real VNTOS subsystem, not by writing disconnected pseudocode.

## Baseline you must preserve

Before you change anything, build and boot the current kernel. Verify:

- the Multiboot2 ELF is accepted by `grub-file`;
- QEMU reaches `VNTOS_COMMAND_LINE_READY`;
- `help`, `about`, `echo test`, `mem`, and `irq` work;
- the timer count increases between two `irq` commands;
- unknown commands still produce the expected error.

Keep a recoverable Git branch. Kernel changes can break boot long before the compiler can tell you what went wrong.

## Pick one extension

Choose exactly one of these projects for the first pass.

### Project A — Better exception reports

Improve `os/interrupts.vnt` so the report names a small, explicit set of exception vectors, such as divide error, invalid opcode, general protection fault, and page fault. Keep a numeric vector and error-code fallback for everything else. Do not attempt to resume execution after a fatal exception.

A correct report should be based on the vector number actually delivered to the dispatcher. Do not label the default marker `255` as though it were the original CPU vector.

### Project B — A timer statistics command

Add an `uptime` or `ticks` command that reuses the same tick counter without printing from interrupt context. Keep the timer handler limited to updating state. Document that a 100 Hz PIT tick is approximate and not a calibrated wall clock.

### Project C — An interrupt-safe event queue design

Write a design note for a fixed-size ring buffer that would allow an IRQ handler to record an event and let the main loop process it later. Define what happens when the buffer is full, which side owns each index, and how interrupt masking or atomic operations prevent races. Do not unmask keyboard IRQ1 until the queue and controller ownership are implemented and tested.

### Project D — Reduce compiler-specific inline assembly

Audit the current use of hard-coded `%rbp` offsets in `os/kernel.vnt` and the device modules. Choose one primitive and propose a compiler-supported interface for it. Include the function signature, target restrictions, expected machine-code behavior, and a regression test. Do not replace a working primitive with unverified inline assembly.

## A proper implementation report

Your change is complete only when the report includes:

1. the exact files changed and why each file owns its responsibility;
2. the behavior before and after the change;
3. the build command and full first failure if the build fails;
4. the QEMU commands and observed output;
5. the relevant GitHub Actions run result;
6. known limitations and one next step.

## VBuild: where it fits

VBuild is useful for ordinary VNT project tasks such as hosted `build`, `run`, `check`, and `test`. The VNTOS kernel is a mixed-language freestanding artifact that needs the VNT compiler, a separate assembler invocation, a custom linker script, and optional GRUB ISO packaging. VBuild's current `elf` target does not replace this pipeline and its task syntax intentionally does not execute arbitrary shell commands.

For the real kernel, use `bash os/build.sh` and the QEMU checks documented above. Do not write a hosted `vbuild.properties` file and assume it will assemble or package the boot image.

## Final checkpoint

Explain the whole path from GRUB to `kernel_main`, then from a PIT event through the IDT and assembly stub into `interrupt_dispatch` and back through `iretq`. If you cannot explain where the CPU state is saved and restored, revisit Lessons 20–24 before adding a scheduler or user mode.

## Definition of done

Use this checklist before asking someone to review your kernel work:

- [ ] The change has one clearly stated goal.
- [ ] Every modified file has a specific responsibility.
- [ ] The VNT and assembly boundary is documented.
- [ ] The baseline build still succeeds.
- [ ] The resulting ELF passes the Multiboot2 check.
- [ ] QEMU reaches the command prompt.
- [ ] The new behavior has a reproducible test.
- [ ] Existing commands and timer ticks still work.
- [ ] The relevant GitHub Actions run has been inspected.
- [ ] Known limitations are written down.
- [ ] The patch does not modify generated build artifacts.
- [ ] No claim of successful verification is made without evidence.

## Example report format

Use this structure in your pull request description:

```text
Goal:
Files changed and responsibility:
Before:
After:
Build command and result:
QEMU command and result:
Relevant CI run:
Known limitations:
Next small step:
```

This format is more useful than listing only filenames because it tells reviewers what behavior changed and how they can reproduce it.

## Keep the scope realistic

Do not attempt a scheduler, userspace, filesystem, heap, keyboard IRQ, and page allocator in one patch. Each depends on invariants that the current kernel does not yet provide. For example, keyboard IRQ handling needs a data queue and an ownership rule for the PS/2 controller; a scheduler needs task state and a deliberate context-switch ABI; a heap needs a physical/virtual memory design and a trustworthy allocator.

A strong systems developer does not measure progress by how many subsystems are named in the README. Measure it by how many invariants are explicit, how many tests detect regressions, and how much code can be explained line by line.

## Choose one project and make the acceptance test concrete

Before implementation, write a small acceptance table:

| Requirement | Evidence |
| --- | --- |
| Existing kernel still builds | successful `bash os/build.sh` result |
| ELF is structurally valid | `readelf` and Multiboot2 check |
| Kernel reaches the shell | QEMU transcript or visible prompt |
| New behavior works | exact command or controlled scenario |
| Regression is covered | repeatable test or CI assertion |
| Limitations are explicit | short section in the report |

For an exception-reporting project, do not deliberately cause a fault in the normal CI boot. Instead, test the formatting logic independently where possible and use a separate disposable QEMU run for a controlled exception. For a timer project, compare numeric values rather than only matching the command label. For an ABI project, inspect the final ELF symbols and add a compiler regression test if the assumption belongs to the compiler.

## Final review questions

- Did you keep policy in VNT and only the architecture boundary in assembly?
- Did you change the smallest set of files that can implement the feature?
- Can you explain the stack, memory, and interrupt assumptions affected?
- Does the test detect the bug you intended to prevent?
- Does the report distinguish observed results from expectations?
- Are future tasks described as future work rather than existing capability?

A strong graduation project is one small subsystem whose behavior is understood, tested, and documented—not a list of features that have not been validated.
