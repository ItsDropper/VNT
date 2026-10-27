# Bits, bytes, and wire formats

## What you will build

Read protocol fields as bytes.

By the end, you should have a concrete change in the companion project, a reproducible test, and an explanation of why the implementation is correct.

Do not start by copying a large block of code. Read the relevant source, write down the invariant, then implement the smallest behavior that can be tested.

## 1. Mental model

Protocols define field widths, byte order, and layout. Never assume a packet is a native struct or that a buffer contains enough bytes.

Think of the implementation as three responsibilities:

1. **Input contract:** what values and state may enter this operation?
2. **Decision:** which checks and transformations are required?
3. **Commit:** what state or output may change only after validation succeeds?

Keep these responsibilities visible. A function that mixes parsing, state changes, and printing is harder to test than focused helpers.

## 2. Repository reconnaissance

Before editing, inspect actual implementation and compiler tests. VNT is experimental, so source code is the authority when a lesson and compiler behavior differ.

1. Open the existing array, string, and module APIs in `lib/std/` and VNT compiler tests.
2. Find the exact declaration for each API you intend to call.
3. Read the implementation, not only the function name.
4. Record parameter types, return type, and failure convention.
5. Check whether a function is marked `unsafe`.
6. Find a working call site and a regression test.
7. Build the compiler using the root README instructions.
8. Compile an unchanged example to establish a baseline.
9. Save exact compiler output if the baseline fails.
10. Do not invent convenience functions that are absent from source.
11. Re-check imports relative to the file containing the import.
12. Record the repository revision used for the exercise.

## 3. State the invariant before writing code

An invariant is a condition that must remain true before and after every public operation.

For this lesson, begin with:

Every accepted field is inside the validated input length; malformed input is rejected before a field is trusted; a failed parse does not expose partial state.

Write the invariant near the state it protects. List which functions can change that state and what each function must check first.

Every public function should have a precondition, success postcondition, and failure postcondition. If those cannot be stated clearly, the interface is not ready.

Make failure atomic: validate inputs and calculate derived values in local variables before changing shared state.

Name failure reasons explicitly. Invalid input, resource exhaustion, checksum failure, and unsupported behavior should not silently collapse into one indistinguishable result.

## Run the current milestone

Compile the integrated offline network simulator from the repository root:

```powershell
.\\vnt.exe --compile examples\\courses\\net-stack\\src\\main.vnt -o net-stack.exe
.\\net-stack.exe
```

Use the test commands in the companion README for the parser boundaries. These exercises use synthetic byte arrays only; no live network is involved. Capture actual output rather than assuming the current compiler accepts an unverified change.
## 5. Implementation recipe

### Step A — identify ownership

Decide which module owns the data and which module consumes it. The packet parser owns byte-offset validation; application handlers receive validated fields rather than re-reading raw offsets.

### Step B — document the contract

List valid inputs, invalid inputs, success results, and failure results. State whether a failed call can mutate state.

### Step C — validate before access

Reject negative lengths, unsupported values, and impossible ranges before calling an unsafe primitive or reading a field.

### Step D — calculate in temporary variables

Do not increment a cursor or publish parsed fields while still validating. Calculate a candidate result, verify every condition, then commit once.

### Step E — implement one successful path

Keep the first version small enough to explain line by line. Do not add caching, concurrency, or optional features before the core behavior is proven.

### Step F — implement rejection paths

For each validation rule, add a test that deliberately violates it. Confirm the documented failure result is returned.

### Step G — add observability

Report useful values such as request size, validated length, offset, or rejection reason. Do not treat raw pointer values as proof of validity.

### Step H — integrate through the public API

Call the new operation from the entry point. Avoid duplicating its internal algorithm in the caller.

### Step I — rerun earlier tests

New branches can break old behavior. Run the complete relevant test set, not only the newest case.

## Worked implementation: Read big-endian fields by offsets

Ethernet EtherType occupies bytes 12 and 13. The high byte contributes 256 times its value; the low byte contributes its value.

```vnt
let ether_type: int = frame[12] * 256 + frame[13]
print(ether_type)
```

For IPv4 EtherType, the two bytes are 8 and 0, giving 2048 (0x0800). Never read them until the frame length is at least 14.

## 6. Test design

Write explicit tests for the applicable categories:

- **Normal input:** intended behavior succeeds.
- **Minimum input:** smallest valid value follows the contract.
- **Boundary input:** exact final valid offset, capacity, or length.
- **One past the boundary:** rejection is safe.
- **Malformed input:** inconsistent fields are rejected.
- **Failure atomicity:** state remains unchanged after rejection.
- **Repeated calls:** state remains coherent across calls.
- **Integration:** the feature works through the public module boundary.

Give each test a stable name, explicit expected result, and failure message identifying the invariant that broke.

## 7. Debugging workflow

1. Reproduce the failure with one fixed input.
2. Record input and expected result.
3. Find the first point where actual state differs.
4. Inspect caller and callee contracts.
5. Change one thing only.
6. Recompile and rerun the reproducer.
7. Run all prior regression tests.
8. Keep the reproducer as a permanent test.

Do not test questionable memory by dereferencing it. A crash-free execution does not prove a memory access was valid.

## 8. Common failure modes

- **Coding before reading the API:** names can hide ownership and failure behavior.
- **Updating state early:** later rejection leaves the module inconsistent.
- **Testing only successful calls:** boundary bugs remain invisible.
- **Mixing policy and parsing:** logic becomes hard to reuse.
- **Ignoring return values:** an allocation or write may have failed.
- **Using magic offsets:** layouts become difficult to audit.
- **Hiding unsupported cases:** callers may assume incomplete behavior is complete.
- **Changing many modules at once:** regression causes become difficult to isolate.

## 9. Apply the lesson to the project

Use byte arrays and explicit offsets. Every parser must check length before reading a field.

Make the change in `examples/courses/net-stack/`. Keep implementation in its owning module, demonstration in the entry point, and regression cases in tests. If the language lacks a feature you expected, adapt to supported syntax and record the limitation instead of inventing syntax.

## 10. Exercise

Write a field table for a 20-byte IPv4 header: offset, width, meaning, and byte order.

Add one additional case not covered by the starter. Explain the bug it would catch and the expected result before running it.

## 11. Review questions

1. What invariant does this operation preserve?
2. Which inputs must be rejected?
3. What is the last failure point before state changes?
4. Which API behavior did you verify in source?
5. Which test proves the boundary is correct?
6. How would you reduce a failure to a deterministic reproducer?
7. What does this implementation explicitly not support?

## 12. Completion checklist

- [ ] I inspected the implementation, not just the API name.
- [ ] I can explain every validation check.
- [ ] I added a success test and boundary/failure tests.
- [ ] A failed operation leaves state unchanged.
- [ ] The entry point calls the module rather than duplicating internals.
- [ ] I ran the relevant compiler and test commands and recorded the actual result.
- [ ] I documented unsupported behavior.

## Checkpoint

What happens if a parser reads a field before checking packet length?

Before moving on, record the packet fixture, expected result, and validation rule. Keep all tests offline. If VNT syntax differs from the lesson, check current compiler tests and standard-library sources rather than inventing syntax.

Explain the change to another beginner without saying only that it "works". Describe the invariant, validation order, success path, failure path, and the regression test.

**Why this matters:** A protocol parser processes untrusted bytes; correctness depends on length checks, explicit field rules, and rejection tests.

Continue only when you can explain both what the code does and why each potentially unsafe or state-changing operation is valid.
