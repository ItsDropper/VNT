# Lesson 14 — Read the Serial Shell

**Goal:** understand the command loop in `kernel_main`.

## 1. The shell is a loop, not a separate process

The current shell runs inside the kernel. It is not a user-space program and has no process isolation. `kernel_main` initializes serial output, prints startup text, reserves space for a command buffer, and enters a loop that receives characters.

Because the kernel is not multitasking, the shell's busy-waiting input routine occupies the CPU while it waits for input.

## 2. Collecting a command

The loop calls `serial_getc()` to read one character. For ordinary printable characters, it appends the character to the buffer if the current length is below the limit, then echoes the character back to the terminal.

Backspace and delete are handled by decreasing the length and sending terminal control characters that visually erase the last character. Enter marks the end of the command, adds a null terminator, and begins command dispatch.

Bounds checks matter. Without a length limit, long input could write past the reserved buffer and corrupt adjacent kernel state. The current code caps the input at 127 characters in a 128-byte buffer.

## 3. Dispatching commands

The shell uses string helper functions:
- `str_eq(left, right)` checks whether two null-terminated strings are equal;
- `starts_with(text, prefix)` checks whether a string begins with a prefix;
- `serial_puts_offset(text, offset)` prints from a selected character offset.

The command loop checks for `help`, `about`, `clear`, `mem`, `echo `, `reboot`, and `halt`. If no recognized command matches and the line is not empty, it prints an error. After most commands, it clears the buffer length and prints the prompt again.

## 4. What the commands actually mean

- `help`: prints the built-in command list.
- `about`: prints a short description of the kernel.
- `clear`: emits terminal escape sequences; whether they clear the display depends on the terminal.
- `echo TEXT`: prints the text after the prefix.
- `mem`: prints linker symbols and the configured identity-map range, not actual free RAM.
- `reboot`: attempts a legacy keyboard-controller reboot sequence.
- `halt`: disables interrupts and halts the CPU in a loop.

The reboot path depends on emulated or real hardware behavior and should not be assumed portable. In QEMU, stop the VM from the host if a command leaves it unresponsive.

## 5. Why string parsing needs care

The current command parser is deliberately simple. It checks exact commands and one prefix. It does not implement quoting, arguments, environment variables, pipes, or a command registry. Future changes should keep input bounded and avoid reading beyond the buffer.

## Checkpoint

Explain the difference between the command buffer's length and its null terminator. Why does the shell set `buffer[length] = 0` before comparing command names?

## Exercise

Trace the `echo HELLO` command. Identify where the prefix is recognized, where the offset is chosen, and where the output reaches the UART.