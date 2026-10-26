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

## 6. Treat input as bounded data

A command buffer has finite capacity. The shell must define what happens when the user types more bytes than fit: ignore extra characters, reject the line, or truncate it with a visible warning. Writing beyond the buffer is not an acceptable behavior. When editing input logic, identify the capacity and the index update, then verify that every write occurs only after the bounds check.

Backspace is another boundary case. It should remove a character only when the current line is nonempty, and the cursor/display behavior should match the buffer state. Empty input should not accidentally dispatch a stale command left in the buffer from a previous line.

## 7. Parse commands deliberately

The current shell uses a small command set and simple string comparisons. That is sufficient for a teaching kernel, but it is not a general command language. Adding arguments requires a defined grammar: where the command ends, how spaces are handled, whether repeated spaces matter, and what happens to unknown arguments.

Keep parsing separate from device operations. The parser should identify a command and its arguments; the command implementation should perform the requested operation; the console should display the result. If every command implements its own byte-by-byte parsing, behavior becomes inconsistent and harder to test.

## 8. Interrupt context is not shell context

The shell runs in the main kernel loop. The timer interrupt handler can run while that loop is executing. Do not call the shell parser from the timer handler or print a routine message for each tick. Interrupt code must be short and predictable; shell work can wait until the main loop reads the counter.

The current keyboard driver still polls PS/2 data. Do not enable the keyboard IRQ while leaving both the polling loop and interrupt handler able to read the same data port. That would create two competing consumers and could lose keystrokes.

## 9. Add a command with a narrow contract

A good new command has a clear name, defined inputs, defined output, and no hidden side effects. For example, a diagnostic command should report existing state rather than mutate the timer, interrupt table, or controller. Update the help text at the same time as the dispatch branch so the interface stays discoverable.

Test the command after normal startup and after another command. Also test an unknown command and an empty line. This catches stale-buffer and dispatch fall-through errors that a single happy-path test misses.

## Lab: define a command before coding

Write a short contract for a hypothetical `ticks` command: accepted syntax, output format, behavior if the counter is zero, and whether it changes state. Then trace the existing `irq` command and compare the implementation with your contract. Do not add a second alias unless it improves a real interface need.

## 11. Keep the command loop predictable

A small shell is easier to maintain when each iteration has a simple contract: begin with a clean input buffer, collect one line within its capacity, handle editing keys, terminate the line, dispatch exactly one command, then reset state for the next line. If state is not reset consistently, the next command may contain bytes from the previous one.

When changing the loop, test consecutive commands, an empty line, backspace at the beginning, unknown input, and a line that reaches the maximum length. Test after a command that prints output as well as after one that changes or reads state. This catches bugs hidden by a single fresh-boot happy path.

## Lab: describe one state transition

Draw the shell states for waiting for a character, appending a character, handling backspace, receiving Enter, dispatching a command, and clearing the buffer. For every transition, write the condition and the state change. Use this state diagram before adding multi-argument parsing or command history.
