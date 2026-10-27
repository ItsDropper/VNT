# Lesson 17 — Make Your First Kernel Change

**Goal:** add one harmless command to the VNTOS serial shell, then verify it.

This is the first lesson where you modify the kernel. Use a branch or another recoverable copy of your work. Do not edit the assembly bootstrap for this exercise.

## 1. Choose a small feature

Add a command named `version` that prints a short version message. This does not require a new driver or hardware interface; it reuses the existing command parser and serial output function.

Before editing, boot the unmodified kernel and confirm the current commands work. That gives you a baseline.

## 2. Find the command dispatch section

Open `os/kernel.vnt` and find the `if str_eq(buffer, "about")` branch inside `kernel_main`. Nearby branches handle other commands.

The pattern looks like this:

```vnt
if str_eq(buffer, "about") {
    serial_puts("VNTOS: VNT-compiled freestanding kernel on x86-64.\r\n")
    handled = 1
}
```

This example shows the existing dispatch style. Your new branch should use the same pattern and should be placed near the other exact-name commands.

## 3. Add the branch

Add a separate condition for `version` that:
1. compares `buffer` with `"version"`;
2. prints a short version message ending with `\r\n`;
3. sets `handled = 1`.

Use syntax that the current freestanding backend supports. Avoid adding a new runtime API, array, pointer operation, or inline assembly to this first change.

Also update the string printed by the `help` command so it lists `version`. Keep the help list and the implemented commands consistent.

## 4. Build and boot

From the repository root:

```bash
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

At the prompt, run:

```text
help
version
about
echo test
not-a-command
```

Expected behavior:
- `help` lists `version`;
- `version` prints the new message;
- existing commands still work;
- an unknown command still produces the unknown-command message.

## 5. Review the diff

Before calling the change finished, confirm:
- only the intended source and help text changed;
- no assembly or linker changes were made;
- the command buffer bounds logic is unchanged;
- the build succeeded;
- you actually booted the resulting ISO and tested the new command.

If compilation fails, fix the first reported error. If it builds but does not boot, return to Lesson 15 rather than adding unrelated changes.

## Checkpoint

Explain why setting `handled = 1` matters and what the shell would print if the command branch did not match.

## Stretch exercise

Add a second command that prints a static line of text. Do not add arguments or memory operations yet. Repeat the same test routine.

## 6. Example change: add a read-only diagnostic command

A good first feature reads state that already exists rather than changing hardware configuration. The current `irq` command is an example: it reports the timer counter without resetting it. For a new command, first identify the state owner and the function that reads it. Do not access a controller port directly from the shell just because the value is hardware-related.

Use this implementation sequence:

1. Write the command contract and expected output.
2. Find the shell dispatch logic in `os/kernel.vnt`.
3. Add one branch using the existing module API.
4. Update `help` output so the command is discoverable.
5. Rebuild and boot in QEMU.
6. Run the new command twice and check that reading it does not mutate the value.
7. Test an unknown command and a second command afterward.
8. Review the diff for unrelated edits.

This sequence tests behavior as well as integration with the shell.

## 7. Know when a change is not small

Changing the timer handler, interrupt stack layout, page-table entries, or linker start address is not an ideal first patch. Those components have architectural preconditions and can fail before useful output is available. Read the relevant lesson and establish a known-good baseline before modifying them.

A patch is small when it changes one understandable contract—not merely when it touches few lines. A two-line change to the interrupt return path can be riskier than a twenty-line help-text update.

## Lab: prepare a review-ready patch

Write a short description with the intended behavior, exact files changed, build command, QEMU test, observed output, and known limitations. Run `git diff --check` and inspect the full diff. If you cannot explain every changed line, reduce the patch before asking for review.

## 8. Worked patch: add a `ticks` command alias

This is a teaching exercise that adds a second command for reading the same timer counter. It does not change the interrupt subsystem. In `os/kernel.vnt`, find the help string and the existing `irq` branch. Add `ticks` to the advertised command list, then add a branch beside `irq`:

```vnt
if str_eq(buffer, "ticks") {
    console_puts("Timer ticks: ")
    console_hex(kernel_timer_ticks())
    handled = 1
}
```

The shell has already collected and terminated the input line before it reaches this code. `str_eq` checks the complete command name. `kernel_timer_ticks()` reads the named counter interface implemented by `os/kernel/core.vnt`. `console_hex` formats the value, and `handled = 1` prevents the shell from printing the unknown-command response.

This is intentionally a read-only command. Do not reset the counter from the shell because that would introduce a shared-state mutation and change the meaning of observations taken elsewhere.

## 9. Verify the change and understand its limits

Build with `bash os/build.sh`, boot the new ISO in QEMU, and run `help`, `ticks`, `irq`, and an unknown command. Both timer commands should report the same value if run close together; the count may increase between calls because IRQ0 continues to arrive.

If the code compiles but the command is unknown, check that the branch was added inside the Enter-handling dispatch block and before the fallback test. If the output is missing or malformed, inspect the console call and whether `handled` is set. If the build fails, fix the first compiler or linker error before investigating QEMU.

After testing, inspect `git diff --check` and the full diff. This patch should touch only the shell source and its help text. The lesson is complete when you can explain every line and reproduce the behavior from a fresh build.
