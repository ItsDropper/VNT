# Build a cursor-aware terminal line editor

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

The shell now edits a bounded command line in place rather than supporting only append-at-end and backspace. This lesson follows the PS/2 scan-code path through the VNT shell and explains insertion, deletion, Home/End, cursor invariants, and the QEMU keyboard regression test.

Implementation files: `os/ps2.vnt`, `os/kernel/shell.vnt`, and `.github/workflows/vntos-boot.yml`. The terminal remains text-only: VGA text memory and COM1 serial output. Keyboard input is still polled; this change does not enable IRQ1.

## Key values are not characters

The PS/2 driver recognizes the Set 1 extended-key prefix `0xE0`. It consumes the prefix and maps the following make code to an internal value:

| Key | Set 1 code after E0 | Internal value |
| --- | --- | ---: |
| Up | `0x48` | 257 |
| Left | `0x4B` | 258 |
| Right | `0x4D` | 259 |
| Delete | `0x53` | 260 |
| Home | `0x47` | 261 |
| End | `0x4F` | 262 |

These values are outside the printable byte range. The shell can distinguish navigation from ASCII input without inserting control values into the command buffer. Key-release codes are ignored. Unknown extended keys are consumed rather than leaking into the command line.

## The line-editor invariant

The input buffer has a maximum of 127 characters. Two values describe the active line:

- `length`: number of characters in the buffer, excluding the terminating zero.
- `cursor`: insertion point, always between zero and `length`, inclusive.

The invariant is:

```text
0 <= cursor <= length <= 127
buffer[0 .. length-1] = visible command
buffer[length] = 0
```

The hardware text cursor must match the logical `cursor`. The dedicated `console_cursor_left()` operation moves it without erasing a character; it also sends ANSI CSI-left to serial output. Left and Right update both; Home moves to zero; End moves to `length`. The editor does not support multiline commands, arbitrary screen scrolling while editing, or Unicode grapheme movement.

## Insert without overwriting

When a printable character arrives and the buffer is not full, shift the suffix one position to the right, starting at the end:

```vnt
let shift_index: int = length
while shift_index > cursor {
    buffer[shift_index] = buffer[shift_index - 1]
    shift_index = shift_index - 1
}
buffer[cursor] = character
length = length + 1
buffer[length] = 0
```

Shifting backwards prevents overwriting characters that still need to move. The shell redraws the suffix, clears the stale last cell, returns the display cursor to the insertion point, then advances one position past the inserted character.

## Delete and Backspace are different

**Delete** removes the character at `cursor`. **Backspace** removes the character immediately before it and moves the cursor one place left. Both shift the remaining suffix left and terminate the buffer again.

The helper `shell_redraw_tail(buffer, cursor, length)` redraws only the changed suffix. It prints the suffix, writes one space to erase the old final character, and calls `console_cursor_left()` enough times to return to the logical insertion point. This is not the same as Backspace: VGA Backspace erases the previous cell. The cursor helper updates the VGA hardware cursor without touching text and sends the ANSI CSI-left sequence to the serial terminal.

This strategy is intentionally simple. It assumes the terminal handles backspace as cursor movement and that the edited command fits the bounded input buffer. It is not a general ANSI terminal implementation.

## History recall and clearing the old line

Up-arrow still recalls one previous non-empty command. Before loading it, the shell moves to the beginning of the line, overwrites the old visible characters with spaces, returns the cursor to the beginning, and then prints the recalled command. The logical buffer, `length`, and `cursor` must be updated together.

Do not repeatedly write a space and immediately backspace when clearing a whole line: that revisits the same cell. Advance while writing spaces, then move the cursor left by the number of cells cleared without erasing those spaces. The QEMU test exercises recall after the editor has moved through its navigation path.

## Automated keyboard regression

The `.github/workflows/vntos-boot.yml` workflow injects keys through QEMU's keyboard monitor. It now checks two editing scenarios before testing history and the other shell commands:

1. Type `ech test`, press Home, move right three places, insert `o`, and execute `echo test`.
2. Type `echo ttest`, move to the character after the space, press Delete, and execute `echo test`.
3. Type `echo ttest`, press End, move left four places, use Backspace, and execute `echo test`.
4. Press Up and Enter to recall and execute the previous command.
5. Run `history`, `uptime`, `synctest`, and the existing memory, IRQ, mouse, and SMP diagnostics.

The workflow assertions are the evidence. A source change or a plausible expected transcript does not prove that the kernel compiled or booted.

## Limits and next steps

- The line is bounded to 127 characters.
- History stores one command only.
- Down-arrow history navigation and command completion are not implemented.
- Serial input remains byte-oriented; ANSI escape sequences from a serial terminal are not decoded as navigation keys.
- Input polling can miss events if another path consumes the PS/2 controller's output byte. A future IRQ-driven driver needs a defined input queue and synchronization contract.
- The shell and console are not re-entrant. Do not call this editor concurrently from APs or interrupt handlers.
- Cursor behavior at the end of a VGA row and while scrolling must be tested in QEMU; serial terminals can differ in their control-character behavior.

A sensible next step is to extract a small line-editor state machine from shell command dispatch and add a bounded command table/help output. Do not enable keyboard IRQs until the input queue, IRQ acknowledgement, and shared-state ownership are defined.

## Exercises

1. Why must insertion shift bytes from right to left?
2. Explain why Delete and Backspace change different buffer positions.
3. Prove the cursor invariant after insertion, deletion, Home, End, and history recall.
4. What goes wrong if the screen cursor is moved but the logical cursor is not?
5. Extend the QEMU test with a backspace-in-the-middle case and verify the exact command executed.
6. Explain why VGA/serial output agreement is not guaranteed for every terminal emulator.

## Acceptance checklist

- [ ] Extended key codes are not inserted as printable characters.
- [ ] Left, Right, Home, End, and Delete update the cursor predictably.
- [ ] Insertion shifts the suffix without losing characters.
- [ ] Backspace removes the previous character; Delete removes the current one.
- [ ] History recall clears the complete old line and restores the logical cursor.
- [ ] The input remains NUL-terminated and capped at 127 characters.
- [ ] CI exercises insertion, Delete, Up-arrow recall, and shell output in QEMU.
- [ ] The English and Finnish lessons describe the same code paths and limitations.

---

[← Previous lesson](./33-terminal-history-and-uptime.md) · [Course index](./README.md) · [Next lesson →](./35-terminal-decimal-uptime.md)
