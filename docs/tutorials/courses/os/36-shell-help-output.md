# Make shell help a maintained interface

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

A shell's help output is part of its user interface. It must describe the commands that actually exist, group related commands so users can find them, and remain covered by a regression check. This lesson updates the existing VNTOS shell; it does not introduce a new command parser or claim that the shell supports arguments for every command.

## Implementation

Open `os/kernel/shell.vnt` and find the `help` branch in `shell_run()`. The shell currently uses explicit `str_eq` checks for fixed commands and `starts_with(buffer, "echo ")` for the one command that accepts trailing text. That is a deliberately simple dispatch strategy: command behavior remains visible and easy to trace in this early kernel.

The help output now separates commands into three groups:

- **Commands:** `help`, `about`, `clear`, `echo`, `history`, `uptime`.
- **Diagnostics:** `synctest`, `mem`, `memtest`, `vmtest`, `irq`, `mouse`, `smp`.
- **Power:** `reboot`, `halt`.

A final usage line explains that `echo TEXT` prints the supplied text and `history` shows the previous completed command. These labels are plain console text; they do not depend on a terminal UI library or dynamic memory.

## Why not introduce a command table yet?

A descriptor table can eventually hold command names, descriptions, and handlers. However, a handler table requires reliable function-pointer and indirect-call support in the freestanding compiler, plus a clear policy for parsing arguments. Do not introduce that architecture just to shorten a list of `if` statements. First verify the language and code generator can represent and call the required function types correctly.

For now, the command checks remain explicit. The help text is grouped for users, and CI asserts that the advertised categories are present. This makes a mismatch visible without pretending that a metadata table is already the source of truth.

## Regression coverage

The workflow `.github/workflows/vntos-boot.yml` boots the kernel in QEMU, sends `help` through the emulated PS/2 keyboard, and captures the serial transcript. The test checks for the three category lines and the usage line. This protects the visible interface against accidental deletion or wording drift.

The check is intentionally narrow: it verifies that expected text appears in the transcript. It does not prove every listed command succeeds, that every command is implemented correctly, or that all hardware works on physical machines. Existing separate transcript checks cover several command paths, while self-test output covers selected memory, synchronization, and virtual-memory behavior.

## Manual verification

Build and run the kernel only in the project's supported QEMU workflow or a disposable QEMU environment. At the prompt, type:

```text
help
```

Expected shape:

```text
Commands: help, about, clear, echo, history, uptime
Diagnostics: synctest, mem, memtest, vmtest, irq, mouse, smp
Power: reboot, halt
Usage: echo TEXT prints TEXT; history shows the previous command.
```

Then try `echo hello` and `history`. The first should print `hello`; the second should identify the preceding completed command according to the shell's one-entry history behavior. Do not use `reboot` or `halt` until you are ready to stop that QEMU session.

## Exercises

1. Add a description for `uptime` that accurately states it uses nominal PIT ticks rather than a calibrated wall clock.
2. Explain why the `echo` branch uses a prefix check while most commands use exact equality.
3. Add a new command and update help plus the QEMU transcript assertion in the same change.
4. Describe the additional compiler support and tests needed before switching to a table of function pointers.
5. Explain why matching a help string in CI does not prove that the corresponding command works.

## Acceptance checklist

- [ ] Help lists all current shell commands in readable groups.
- [ ] The usage line accurately describes `echo` and `history`.
- [ ] QEMU CI checks each help group and the usage line.
- [ ] Command dispatch remains explicit; no unsupported function-pointer feature is assumed.
- [ ] Documentation distinguishes transcript assertions from full functional or hardware validation.

---

[← Previous lesson](./35-terminal-decimal-uptime.md) · [Course index](./README.md)
