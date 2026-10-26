# Lesson 13 — Serial Output and Input

**Goal:** understand the kernel's first practical device interface.

## 1. Why serial?

A serial console sends and receives bytes. It is simpler than a graphical display and is useful when debugging before graphics or a full input stack exists. QEMU can connect the guest's serial port to the host terminal with `-serial stdio`.

VNTOS currently uses the legacy COM1 UART, commonly accessed at I/O base address `0x3F8`. The port numbers are hardware interface addresses, not ordinary RAM pointers.

## 2. Initialization

Read `serial_init()` in `os/kernel.vnt`. It configures several UART registers, including the divisor latch, line control, FIFO control, and modem control. The setup chooses a divisor and then configures the line for basic serial communication.

This code assumes the expected legacy UART interface is available. A production driver would need stronger device discovery, timeout/error handling, and clear abstractions for the hardware.

## 3. Sending a byte

`serial_putc(value)` waits until the UART reports that its transmitter holding register is ready, then writes the byte to the data port. The status register is read from port `0x3FD`; the data register is at `0x3F8`.

The polling loop is a **busy wait**: the CPU repeatedly checks the device instead of sleeping or receiving an interrupt. That is acceptable for a tiny early console but inefficient as a general device-driver design. VNTOS now has a PIT timer IRQ, but COM1 itself is still polled; the existence of an IDT does not turn every driver into an interrupt-driven driver.

## 4. Receiving a byte

`serial_getc()` waits until the UART reports that a byte is available, then reads it. This is also a busy wait. If no input arrives, the function does not return. That behavior explains why the shell waits for you to type something.

The current inline assembly writes a result through a compiler-specific stack-frame offset. That is fragile; a future compiler change should replace it with a supported output constraint or intrinsic. Do not interpret the current implementation as a recommended pattern for all VNT functions.

## 5. Sending strings

`serial_puts(text)` loops through characters until it reaches a zero byte and sends each one. The shell uses it to print the prompt, help text, and responses.

The code assumes the string data is valid and null-terminated. This simple model is adequate for the current fixed strings, but string handling in a freestanding environment needs careful bounds and memory rules.

## 6. Try the console

Build and boot in QEMU:

```bash
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -display none -serial stdio
```

Type `help` and press Enter. If the startup message appears but typed characters do not, distinguish a receive/input problem from a transmit/output problem.

## Checkpoint

Explain what the transmitter-ready check does, what the receive-ready check does, and why busy waiting is a limitation.

## Exercise

Trace a character typed in QEMU from `serial_getc()` to the command buffer. Do not change UART register values without checking the hardware behavior and QEMU test results.

## 7. Why polling works—and where it stops scaling

The UART driver polls a line-status register until the transmitter can accept a byte. Polling is straightforward because the caller controls when it checks the device, and the code needs no interrupt queue. Its disadvantage is that the CPU may repeatedly check a device that is not ready. A broken or absent device can also leave the kernel spinning forever unless the driver defines a timeout.

Polling is appropriate for a tiny boot console because it is easy to debug and available before more advanced scheduling exists. It is not a general solution for high-throughput or power-efficient device I/O.

## 8. Serial output is a diagnostic channel

Serial output can reveal early boot progress even when VGA is not initialized or the display is hidden. Keep messages short and make the stage obvious. A marker immediately before and after a risky initialization step can tell you whether execution reached that point.

Do not print from every timer interrupt. COM1 is polled, so each character may wait for hardware readiness. Printing from an IRQ can keep the CPU inside the handler for a long time and interleave output with the shell. The timer handler should update a counter; the shell can format and display that value later.

## 9. Test both directions

Output tests show whether the transmit path works; they do not prove input works. Input tests show whether bytes arrive; they do not prove command parsing handles a complete line. Test one byte, a short string, an empty line, a longer line near the buffer limit, backspace, and repeated commands. Use the exact QEMU serial configuration documented in `os/README.md`.

For every test, state what the expected result is. If a key is typed into the graphical QEMU window, that exercises the emulated PS/2 keyboard path; typing into the host terminal connected to serial exercises the serial-input path. These are distinct paths.

## 10. Failure modes to consider

A missing serial message can be caused by an uninitialized UART, the wrong QEMU serial route, a transmit-ready loop that never completes, or code that never reached the print call. A garbled message can point to baud/divisor configuration or concurrent output. A frozen shell after input may be a receive-polling issue or a parser bug.

When debugging, temporarily reduce output to one marker and confirm the channel. Then restore the normal message. Do not rewrite the driver until you have evidence that the failure is in the driver.

## Lab: build a serial test matrix

Create rows for one-byte transmit, multi-byte transmit, one-byte receive, backspace, empty input, and a line near the input buffer's limit. For each, record the command used, expected output, actual output, and whether VGA or serial was the active path.
