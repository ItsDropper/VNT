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