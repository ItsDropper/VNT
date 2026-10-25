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