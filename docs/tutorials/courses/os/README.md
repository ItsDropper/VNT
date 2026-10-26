# Build VNTOS: A Beginner's Operating-System Course

**Goal:** understand, build, boot, inspect, and safely extend the real VNTOS
kernel in `os/`. This course starts at the bootloader and ends with a modular
VNT kernel that handles CPU exceptions and a periodic timer interrupt.

This is a practical course, not a claim that VNTOS is a finished desktop OS.
The current kernel has a Multiboot2 bootstrap, a 1 GiB initial identity map,
a VNT serial/VGA console, polled PS/2 input, an IDT, fatal exception reporting,
and a PIT timer IRQ. It does **not** have a scheduler, userspace, filesystem,
general heap allocator, or general device-driver framework.

## Who this is for

No prior OS-development experience is required. Basic terminal use helps.
Read the early lessons in order: later lessons depend on concepts introduced
earlier. Every implementation lesson points to the real files, explains the
reason for each boundary, gives exact commands, and includes a check you can
perform instead of relying on a vague claim that it works.

## Safety and working rules

- Use QEMU for experiments; do not install this kernel on your physical PC.
- Never write an image to a USB drive or disk containing important files.
- Run build commands from the repository root unless a lesson says otherwise.
- Inspect the first build error. Later linker errors can be consequences.
- A successful compile is not proof of a successful boot or interrupt.
- The current VNT freestanding compiler subset is experimental. If the course
  and the source disagree, the source and current CI behavior win.
- Make one change at a time and retain a recoverable Git branch.

## How to study a lesson

1. Read the goal and vocabulary.
2. Open the exact source file named by the lesson.
3. Trace the call path before editing.
4. Run the command shown and compare the observed output with the expectation.
5. Answer the checkpoint in your own words.
6. Make the exercise change only after you can explain its failure modes.
7. Rebuild and boot in QEMU; inspect GitHub Actions for the corresponding run.

## Course map

### Phase 1 — Foundations and setup
1. [Start here: your first kernel lab](00-start-here.md)
2. [What an operating system does](01-what-is-an-os.md)
3. [The machine underneath the OS](02-computer-basics.md)
4. [Set up the safe development environment](03-setup.md)
5. [Learn just enough VNT for kernel code](04-vnt-for-kernels.md)

### Phase 2 — Boot, CPU state, and the ELF
6. [Follow the build pipeline](05-build-pipeline.md)
7. [Understand the boot sequence](06-the-boot-sequence.md)
8. [Understand GRUB and Multiboot2](07-grub-and-multiboot2.md)
9. [Read the assembly bootstrap](08-reading-boot-assembly.md)
10. [Enter x86-64 long mode](09-long-mode.md)
11. [Understand the initial page tables](10-initial-paging.md)
12. [Understand ELF and the linker script](11-elf-and-linker.md)

### Phase 3 — Freestanding VNT and the original shell
13. [Hosted programs versus freestanding kernel code](12-freestanding-vnt.md)
14. [Serial output and input](13-serial-io.md)
15. [Read the command shell](14-the-serial-shell.md)
16. [Debugging boot failures](15-debugging.md)
17. [Test changes without guessing](16-testing.md)
18. [Make your first kernel change](17-first-kernel-change.md)
19. [What comes next?](18-next-steps.md)

### Phase 4 — A real modular kernel and interrupts
20. [Split the kernel into VNT modules](19-split-the-kernel-into-vnt-modules.md)
21. [Interrupts, exceptions, and the IDT](20-interrupts-and-the-idt.md)
22. [The small assembly boundary](21-minimal-assembly-entry-stubs.md)
23. [Keep interrupt policy in VNT](22-vnt-interrupt-dispatch.md)
24. [The PIT, PIC, and timer ticks](23-pit-pic-and-timer-ticks.md)
25. [Build and verify the interrupt kernel](24-build-and-test-the-interrupt-kernel.md)
26. [Graduation project: improve a subsystem](25-graduation-project.md)

## What you should be able to do at the end

You should be able to explain the complete path from GRUB to `kernel_main`,
follow an import from `kernel.vnt` into a device module, describe why the IDT
and interrupt-entry assembly exist, trace IRQ0 from the PIT through the IDT
and VNT dispatcher back to `iretq`, run the `irq` command, and produce
evidence that the kernel still builds and boots.

## Source map

| File | Responsibility |
| --- | --- |
| `os/boot.S` | Multiboot2 header, early stack, paging, long-mode transition |
| `os/kernel.vnt` | Shared state, CPU control, kernel entry, shell policy |
| `os/serial.vnt` | COM1 UART driver |
| `os/console.vnt` | VGA text output and combined console |
| `os/ps2.vnt` | Polled keyboard and mouse input |
| `os/interrupts.vnt` | VNT exception/IRQ dispatch policy |
| `os/interrupts.S` | IDT, entry stubs, PIC/PIT setup, `iretq` boundary |
| `os/linker.ld` | ELF section layout and symbols |
| `os/grub.cfg` | GRUB Multiboot2 configuration |
| `os/build.sh` | Compiler, assembler, linker, and ISO pipeline |

## Build tooling note: VBuild

VBuild is the native build driver for ordinary VNT projects. Its current
freestanding `elf` target does not link this mixed kernel with both assembly
objects, the custom `os/linker.ld`, and the GRUB ISO layout. The authoritative
kernel build remains:

```sh
bash os/build.sh
```

Do not claim that `vbuild build` creates the VNTOS boot image. Read
[`docs/VBUILD.md`](../../../VBUILD.md) for VBuild's supported targets and
limits; use the kernel-specific pipeline for the actual OS.

Start with [Lesson 00](00-start-here.md).

## How to turn a lesson into a real code change

For each implementation lesson, use this repeatable workflow:

1. **Read the current implementation.** Open the named file in the `os/` directory and follow its callers and callees. Do not rely on a copied snippet if the repository has moved on.
2. **State the invariant.** Examples: the IDT base points to a complete table; the shell never writes beyond its input buffer; the timer handler does not block; the page-table entries cover code needed after paging is enabled.
3. **Define the observation.** Decide exactly what you expect to see in the build output, ELF metadata, QEMU window, or serial transcript.
4. **Change one contract.** Keep unrelated cleanup out of the same patch.
5. **Build from the repository root.** Use `bash os/build.sh`, not a stale generated object or a guessed compiler command.
6. **Inspect the artifact.** Use `file`, `readelf`, `nm`, and the Multiboot2 check when relevant to the claim.
7. **Boot in QEMU.** Use the documented command and test the specific behavior plus one existing behavior.
8. **Review and report.** Run `git diff --check`, inspect the full diff, and record what actually happened.

If a step fails, preserve the first failure and stop. Do not continue to later steps with stale artifacts and then report the overall experiment as successful.

## Evidence vocabulary

Use precise status words in notes and pull requests:

- **Built:** the relevant compile/link command exited successfully.
- **Validated:** a specific structural check passed, such as the Multiboot2 header check.
- **Booted:** QEMU executed far enough to reach the observed marker or prompt.
- **Exercised:** the new behavior was invoked.
- **Verified:** the test actually checked the claimed property.
- **Unverified:** the test was not run, did not assert the property, or its result is still pending.

For example, "the `irq` command prints a count" is weaker than "two readings showed the timer counter increasing." Neither statement should be made unless those observations were actually recorded.

## Course completion deliverable

At the end, keep a short technical report with the subsystem diagram, the invariant you changed or preserved, the exact source files involved, the build and QEMU commands, the observed output, the relevant CI run, and the remaining limitations. Include a short explanation of why the assembly boundary remains minimal and which behavior is owned by VNT.

This report is part of the learning goal: kernel development requires not only making a change but being able to explain its assumptions and demonstrate that it works.

## Worked trace: from a shell command to hardware state

The `irq` command is a compact example that crosses the course's main boundaries:

1. The shell in `os/kernel.vnt` recognizes the text `irq`.
2. It reads shared state slot 11 and passes the value to `console_hex`.
3. The timer interrupt entry in `os/interrupts.S` saves registers and calls the emitted VNT dispatcher symbol.
4. `os/interrupts.vnt` handles vector 32 by incrementing slot 11 and returning quickly.
5. The assembly entry acknowledges the PIC, restores registers, and executes `iretq`.
6. The shell later displays the updated counter in ordinary kernel context.

Trace both directions: command-to-output and hardware-event-to-state. This demonstrates why the VNT dispatcher does not print on every tick and why the shell does not need to know the PIC's port addresses.

The shared state array is a compact teaching mechanism, not a finished type-safe kernel API. A future improvement would give the timer count a named interface and define how concurrent reads and writes are synchronized. Do not claim the current design already provides a general concurrency abstraction.
