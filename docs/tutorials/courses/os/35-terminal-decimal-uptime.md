# Print uptime and counters as readable decimal numbers

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Why this matters in a text-only kernel

A diagnostic command is useful only when its output is readable and its units are clear. The shell's `uptime` command previously printed timer ticks and approximate seconds through `console_hex`, so a value such as 100 seconds appeared as `0x64`. Hexadecimal is useful for addresses and register values, but it is an awkward default for everyday status output.

This lesson adds `console_decimal(value)`, a small allocation-free formatter, and uses it for the timer-based uptime report. It does not add a wall clock or make the PIT more accurate.

## The decimal formatter

Implementation: `os/console.vnt`.

```vnt
fun console_decimal(value: i64) {
    if value == 0 {
        console_putc(48)
        console_puts("\r\n")
        return 0
    }
    if value < 0 {
        console_putc(45)
        value = 0 - value
    }
    let divisor: i64 = 1
    while divisor <= value / 10 {
        divisor = divisor * 10
    }
    while divisor > 0 {
        let digit: int = value / divisor
        console_putc(48 + digit)
        value = value % divisor
        divisor = divisor / 10
    }
    console_puts("\r\n")
    return 0
}
```

The function emits ASCII digits directly: character code 48 is `0`, and `48 + digit` is the corresponding character for digits zero through nine. It does not allocate a temporary string or use the heap, which is important because VNTOS does not have a general heap allocator.

### Why the divisor works

For a positive input, the first loop finds the highest power of ten no greater than the value. For `1234`, the divisor becomes `1000`. Each iteration prints the leading digit, removes it with remainder, and divides the divisor by ten:

| Step | Value before | Divisor | Printed digit | Value after remainder |
| --- | ---: | ---: | ---: | ---: |
| 1 | 1234 | 1000 | 1 | 234 |
| 2 | 234 | 100 | 2 | 34 |
| 3 | 34 | 10 | 3 | 4 |
| 4 | 4 | 1 | 4 | 0 |

The growth condition `divisor <= value / 10` avoids multiplying the divisor past the magnitude of the input. Zero needs a separate branch because the digit loop otherwise would not run.

The current implementation is intended for ordinary signed counter values, not the minimum representable signed 64-bit integer: negating that one value cannot be represented as a positive `i64`. Uptime ticks are non-negative, so this edge case is outside the command's expected input.

## Use decimal for uptime, hexadecimal for addresses

Implementation: `os/kernel/shell.vnt`.

The `uptime` command now prints:

- timer ticks in decimal;
- approximate elapsed seconds in decimal;
- a warning that the current clock is a nominal 100-Hz PIT tick count and the seconds are approximate.

The formula is `seconds = ticks / 100`. Integer division truncates the fractional part. The PIT frequency is nominal, not calibrated against a real-time clock, HPET, or invariant TSC. Do not present this value as wall-clock time or as a precise duration measurement.

Use `console_hex` for addresses, masks, APIC IDs, and register-like values where hexadecimal is conventional. Use decimal for counts that users compare or read as quantities. Keep the unit in the label rather than expecting users to infer it from the number.

## Verification

After building and booting through the repository's supported workflow, run:

```text
uptime
mem
irq
```

Expected shape (values vary):

```text
Timer ticks (decimal):
Approximate uptime seconds (decimal):
Clock source: PIT at nominal 100 Hz; seconds are approximate.
```

The first two labels should be followed by decimal digits, not a `0x` prefix. The `mem` command keeps kernel addresses in hexadecimal but now prints usable/free page counts in decimal. The `irq` command now prints timer ticks and IRQ0/IRQ1 counters in decimal as well. Labels explicitly identify the format; CPU IDs and register-like values remain hexadecimal.

Useful formatter test cases are zero, a one-digit number, a power of ten, a number containing internal zeroes, and a large positive value. In a later change, factor formatting into a pure digit-buffer helper so it can be tested without hardware output.

## Exercises

1. Trace the divisor and remainder values for `907`. Why must the first printed digit not be zero?
2. What bug appears if the zero special case is removed?
3. Why does `ticks / 100` lose sub-second information?
4. Which shell values should remain hexadecimal, and why?
5. Design a test seam that lets CI verify exact output for zero, 10, 100, and 907 without booting the kernel.
6. Explain why this formatter should not allocate a heap buffer.

## Acceptance checklist

- [ ] Decimal output handles zero and ordinary positive values.
- [ ] The formatter allocates no memory and uses no libc dependency.
- [ ] Uptime ticks and approximate seconds are printed in decimal.
- [ ] The output explicitly identifies the PIT-based estimate and its limitation.
- [ ] Addresses and register-like diagnostics can continue to use hexadecimal.
- [ ] No claim is made that PIT ticks provide accurate wall-clock time.
- [ ] The English and Finnish course lessons describe the same implementation and limits.

---

[← Previous lesson](./34-terminal-editable-line-editor.md) · [Course index](./README.md) · **Next: add deterministic formatter tests.**
