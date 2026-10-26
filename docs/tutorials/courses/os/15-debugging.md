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

## 6. Diagnose interrupts separately from boot

A kernel can reach the prompt and still have a broken interrupt path. The IDT must be initialized, the descriptor base and limit must be valid, the correct handler stub must be installed, the PIC must be mapped as expected, and the timer IRQ must be unmasked before the tick counter can change. The CPU must also have maskable interrupts enabled at the correct point.

Use the `irq` command twice with a delay. If the counter does not increase, inspect each link in that path rather than changing the shell or timer divisor at random. If the kernel resets, consider an invalid gate, a malformed stack frame, or a fault during exception handling. A triple fault often produces little useful output, so use controlled markers and the QEMU debugger.

## 7. Use the ELF to answer linker questions

When the linker reports an undefined reference, copy the exact symbol spelling from the error. Then inspect the final object/symbol names with `nm` or `readelf -s`. The source-level VNT name may have a compiler prefix, and assembly must reference the emitted name. Also verify that the object defining the symbol appears in the link command.

A successful compile of the VNT source does not guarantee that the assembly object and VNT object agree on their interface. The linker checks symbol resolution; the runtime test checks whether the call contract was correct.

## 8. Preserve the first failure

Do not fix five things at once after a crash. Capture the first error, save the diff, and reproduce the failure with the same command. If the change affects boot assembly, return to the last known-good version and reapply one small change. If it affects VNT policy, test that function in the simplest path available before adding more behavior.

For a serious bug report, include the commit SHA, tool versions, exact build command, first error line, relevant ELF inspection, QEMU command, and last visible output. This is enough information for someone else to start from the same evidence.

## 9. Debugging exercises

- Build succeeds but QEMU resets immediately: inspect the ELF entry point, bootstrap transition, and early markers.
- Serial output works but VGA is blank: inspect display mode and the VGA text-memory path rather than the serial driver.
- Shell works but `irq` remains unchanged: inspect IDT/PIC/PIT initialization and the timer dispatch path.
- Link fails on `vnt_fn_interrupt_dispatch`: inspect the emitted VNT symbol and whether `interrupts.S` calls the exact name.
- Keyboard input works only in one QEMU mode: identify whether the test is using PS/2 or serial input.

These are starting hypotheses, not automatic diagnoses. Confirm each with evidence.

## Lab: write a fault tree

Choose one symptom above and draw a tree of at least three possible causes. For each cause, name one observation that would support or rule it out. Only after the tree is built should you choose the first experiment.
