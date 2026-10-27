# Terminal history and uptime diagnostics

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

Improve the text-based VNTOS shell without adding a graphical desktop or a heap. This lesson documents one-command history recall with the PS/2 Up arrow, a `history` command, an `uptime` diagnostic, and the QEMU regression check for the real keyboard path.

Implementation files: `os/ps2.vnt`, `os/kernel/shell.vnt`, and `os/arch/x86_64/boot.S`. Input remains polled. Output is sent to VGA text memory and COM1 serial.

## Trace the terminal path

1. `keyboard_poll()` reads the 8042 status and data ports.
2. `keyboard_translate()` translates ordinary Set 1 scan codes to character values.
3. Extended PS/2 keys start with the `0xE0` prefix. The driver remembers that prefix until the next scan code.
4. The Up key is returned as internal value `257`, not as printable ASCII.
5. `shell_run()` recognizes 257 and copies the prior command into the live input buffer.
6. `console_putc()` echoes the recalled text to VGA and serial.

The internal key value is outside the byte-sized character range, so it cannot be mistaken for a printable character. Value `256` means “no character available.”

## Handling the E0 prefix

An extended key arrives as multiple scan-code bytes. Returning immediately for every `0xE0` byte loses the meaning of the following byte. The driver uses state slot 177:

```vnt
if state_get(177) == 1 {
    state_set(177, 0)
    if scan == 0x48 {
        return 257
    }
    return 256
}
if scan == 0xe0 {
    state_set(177, 1)
    return 256
}
```

The Up-arrow make code is `0x48`. Release codes are ignored, so holding and releasing Up does not insert a character. Unknown extended keys are ignored instead of corrupting the command line.

## Fixed-size history without a heap

VNTOS does not yet have a general heap or a terminal-line object. The previous command uses the shared scalar-state area declared in `os/arch/x86_64/boot.S`.

| State slots | Purpose |
| --- | --- |
| 176 | Physical allocator next-fit cursor; do not reuse |
| 177 | Extended-key prefix pending |
| 178 | Previous command length |
| 179 | Reserved |
| 180–307 | Previous command bytes, up to 127 characters plus a terminator |

The shell saves a completed non-empty command after dispatching it. That ordering matters: `history` prints the command before it, rather than printing itself. An empty Enter does not overwrite history. The current input limit and history capacity are both 127 characters.

On Up-arrow, the shell visually erases the current line, copies saved bytes into the active buffer, and echoes them. Press Enter to execute the recalled command. This is a one-entry history, not a general history ring.

## Commands and timer meaning

- `history` prints the last completed non-empty command, or reports that history is empty.
- `uptime` prints the raw PIT tick count and an approximate seconds count.
- Up-arrow recalls the previous command into the editable input line.

The PIT is configured for approximately 100 interrupts per second, so `ticks / 100` is only an estimate. It is not wall-clock time. Emulator timing, missed ticks, or future timer changes can affect the estimate. Printing the raw tick count makes the underlying measurement visible.

## Verify the real keyboard path

The workflow `.github/workflows/vntos-boot.yml` boots QEMU with four virtual CPUs and injects keys through QEMU's keyboard monitor. It types `echo test`, presses Up and Enter, then runs `history` and `uptime`. The workflow checks for the recalled command and the new output.

Manual check from the repository root:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -smp 4 -serial stdio
```

Expected interaction:

```text
VNTOS> echo test
test
VNTOS> [press Up]
VNTOS> echo test
test
VNTOS> history
Last command: echo test
VNTOS> uptime
Timer ticks: 0x...
Approximate uptime seconds: 0x...
```

Exact tick values depend on timing. Check the GitHub Actions result for the commit; source changes and plausible output are not proof of a completed QEMU test.

## Failure cases and limits

1. **No history:** Up leaves the line unchanged; `history` reports an empty history.
2. **Empty command:** does not replace the previous command.
3. **Long command:** input is capped at 127 characters.
4. **Key release:** ignored, not returned as a character.
5. **Unknown extended key:** ignored.
6. **Serial navigation:** ordinary serial input still works, but ANSI escape sequences are not parsed as navigation keys.
7. **Timer precision:** the seconds value is approximate and must not be used for deadlines, calendar time, or security decisions.

This is not a complete terminal emulator. It has no multi-entry history, Down-arrow navigation, arbitrary cursor movement, insertion/deletion in the middle of a line, serial ANSI parser, or asynchronous keyboard interrupt handler. IRQ1 remains masked because keyboard input is polled.

The next terminal step should separate a bounded line editor from shell command dispatch, then add cursor movement and deletion with defined VGA/serial behavior. Do not enable keyboard interrupts until the handler and input-buffer ownership are safe.

## Exercises

1. Why is `0xE0` not a complete navigation key by itself?
2. Why return 257 instead of an ASCII byte?
3. Why must state slot 176 remain reserved for the allocator cursor?
4. Why save history after command dispatch?
5. Design a two-entry history without overlapping allocator or SMP state; document every slot.
6. Why is dividing PIT ticks by 100 only an estimate?
7. Add CI assertions only after identifying the exact QEMU output.

## Acceptance checklist

- [ ] Normal Set 1 character translation is unchanged.
- [ ] E0 prefix and Up-arrow make code are handled.
- [ ] Up recalls the last non-empty command.
- [ ] `history` reports the preceding command.
- [ ] `uptime` shows raw ticks and labels seconds approximate.
- [ ] State slots do not overlap allocator or SMP fields.
- [ ] CI checks the actual PS/2 path and new output.
- [ ] English and Finnish lessons document the same behavior and limits.

---

[← Previous lesson](./32-reserved-memory-map-precedence.md) · [Course index](./README.md) · [Next lesson →](./34-terminal-editable-line-editor.md)
