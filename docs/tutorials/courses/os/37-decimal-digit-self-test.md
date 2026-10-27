# Test decimal digit extraction without console output

Copyright (c) 2026 ItsDropper. Licensed under the MIT License; see the repository-root `LICENSE`.

## Goal

A formatting routine is easier to trust when its arithmetic can be tested separately from hardware output. VNTOS now extracts one base-10 digit through the pure helper `decimal_digit_at(value, divisor)` in `os/console.vnt`. The helper does not write to VGA or serial, allocate memory, or mutate kernel state.

## The helper's contract

The helper's caller contract is a non-negative integer and a positive divisor. The intended caller supplies a power of ten: 1, 10, 100, 1000, and so on. It returns:

```text
(value / divisor) % 10
```

Integer division shifts the requested decimal position to the units place; remainder by ten selects the digit there. For example, `decimal_digit_at(907, 100)` returns 9, while `decimal_digit_at(907, 10)` returns 0. The helper rejects a zero divisor with -1. Negative values and negative divisors are outside the current helper contract; do not rely on signed-input validation in this freestanding compiler subset.

The helper does not check whether a positive divisor is actually a power of ten. That is a caller invariant, documented here to keep the kernel helper small. Passing 3, for example, is outside its intended contract even though the arithmetic expression is defined.

## Formatter integration

The existing `console_decimal(value)` function still owns output policy: it handles zero, prints a minus sign for negative inputs, chooses a divisor, emits digits, and ends with CRLF. For each digit, it now calls `decimal_digit_at` instead of embedding the division-and-remainder expression in the output loop.

This separation is useful because digit selection is deterministic arithmetic, while `console_putc` and `console_puts` have hardware side effects. The test can exercise the former without needing a serial terminal or VGA buffer. It is not yet a hosted unit-test binary; the `dectest` shell command runs these assertions inside the booted kernel.

## Run the self-test

At the VNTOS prompt, enter:

```text
dectest
```

Expected output:

```text
Decimal digit self-test: PASS
```

The test covers zero, a power-of-ten position, an embedded zero digit, the units digit, the leading digit of a four-digit value, and a zero divisor. The QEMU workflow sends `dectest` through the emulated keyboard and requires the PASS line in the serial transcript.

A failing assertion prints `Decimal digit self-test: FAIL`. CI checks for PASS, so FAIL or a crash before the expected output causes the workflow to fail. The transcript assertion proves that this test reported success during that run; it does not prove physical hardware compatibility or test every possible 64-bit input.

## Limits and future improvements

- `console_decimal` still cannot safely negate the minimum signed 64-bit value. Do not treat that value as supported.
- The helper assumes the caller uses a power-of-ten divisor; a future API could validate that contract or use a digit-position index instead.
- Because `dectest` executes inside the kernel, it is independent of console output only at the arithmetic-helper level. A standalone host-side test harness would require a supported way to compile and execute the same VNT helper outside the freestanding kernel.
- Keep diagnostic tests deterministic and avoid tests that modify allocator state, APIC routing, or shared SMP state unless they can restore the previous state.

## Exercises

1. Work out the result of `decimal_digit_at(120304, 1000)`. Which digit is selected?
2. Why does `decimal_digit_at(907, 10)` return zero rather than skipping to the next nonzero digit?
3. Explain why a zero divisor must be rejected before division.
4. Extend the test with a two-digit number and the divisor 1.
5. Propose a safe strategy for formatting the minimum signed 64-bit integer without overflowing during negation.

## Acceptance checklist

- [ ] Digit extraction is a separate helper with no console calls.
- [ ] A zero divisor returns the documented error sentinel; callers provide non-negative values and positive divisors.
- [ ] The decimal formatter uses the helper for digit selection.
- [ ] `dectest` covers representative digits and invalid inputs.
- [ ] QEMU CI requires the self-test PASS output.
- [ ] Documentation states the power-of-ten invariant and the remaining minimum-integer limitation.

---

[← Previous lesson](./36-shell-help-output.md) · [Course index](./README.md)
