# Copy and fill

## What you will build

Use bulk operations only on validated ranges

By the end, you should have a concrete change in the companion project, a reproducible test, and an explanation of why the implementation is correct.

Do not start by copying a large block of code. Read the relevant source, write down the invariant, then implement the smallest behavior that can be tested.

## 1. Mental model

mem.copy is overlap-safe according to the documented API; mem.set fills bytes. Neither proves the supplied pointers and lengths are valid.

Think of the implementation as three separate responsibilities:

1. **Input contract:** what values and state may enter this operation?
2. **Decision:** which checks and transformations are needed?
3. **Commit:** what state or output may change only after validation succeeds?

Keep these responsibilities visible in code. A compact function that mixes parsing, state changes, and printing is harder to test than a few focused helpers.

## 2. Repository reconnaissance

Before editing, inspect the actual implementation and compiler tests. VNT is experimental, so source code is the final authority when a lesson and compiler behavior differ.

1. Open `lib/std/mem.vnt` and `docs/raw-memory.md`.
2. Find the exact declaration for every API you intend to call.
3. Read the implementation, not only the function name.
4. Record parameter types, return type, and failure convention.
5. Check whether a function is marked `unsafe`.
6. Find an existing successful call site.
7. Find a regression test for the language feature you need.
8. Build the compiler from the root README instructions.
9. Compile an unchanged example to establish a baseline.
10. Save the exact compiler output if that baseline fails.
11. Do not invent a convenience function that is not present in source.
12. Re-check relative import paths from the file that contains the import.

## 3. State the invariant before writing code

An invariant is a condition that must remain true before and after every public operation. It is stronger than saying that a demo appears to work.

For this lesson, begin with:

`0 <= cursor <= capacity`; every successful range fits in the live backing allocation; a rejected operation leaves allocator state unchanged.

Write the invariant near the state it protects. List which functions can change that state and what each function must check first.

A public function should have a clear precondition, success postcondition, and failure postcondition. If you cannot state those three things, the interface is not ready to implement.

Make failure atomic: validate inputs and calculate derived values in local variables before changing shared state.

If an operation can fail, name each failure reason. Do not use one generic fallback that hides invalid input, resource exhaustion, and unsupported behavior behind the same result.

## Run the current milestone

Compile the multi-file memory project from the repository root:

```powershell
.\\vnt.exe --compile examples\\courses\\memory-lab\\src\\main.vnt -o memory-lab.exe
.\\memory-lab.exe
```

Use the relevant test command from the project README after implementing this lesson. Capture actual output and do not treat compilation alone as proof of memory correctness.
## 5. Implementation recipe

Follow this order instead of changing several modules at once.

### Step A — identify the owner

Decide which module owns the data and which module only consumes it. Only the allocator module should perform pointer arithmetic; callers should receive a reservation result, not mutate the cursor.

### Step B — document the contract

Write down valid inputs, invalid inputs, success results, and failure results. State whether a failed call can mutate anything.

### Step C — validate before access

Reject negative lengths, unsupported values, and ranges that cannot fit before calling an unsafe primitive or reading a field.

### Step D — calculate in temporary variables

Do not increment the cursor or publish parsed fields as you go. Calculate a candidate result, verify every condition, then commit once.

### Step E — implement one successful path

Keep the first implementation small enough to explain line by line. Do not add caching, concurrency, or optional features before the core behavior is proven.

### Step F — add failure paths

For each validation, test the condition that should cause rejection. Confirm that the caller receives the documented failure result.

### Step G — add instrumentation

Report useful values such as the requested size, validated length, offset, or rejection reason. Do not print arbitrary pointer addresses as if they prove validity.

### Step H — integrate through the public API

Call the new function from the example's entry point. Avoid duplicating its internal algorithm in the caller.

### Step I — rerun earlier tests

Every new branch can break earlier behavior. Run the complete relevant test set, not only the newest test.

## Worked implementation: Validate both ranges before bulk operations

The source and destination each need a valid live region. Overlap-safe copying does not validate either pointer.

```vnt
let filled: bool = mem.set(destination, 165, length)
let copied: bool = mem.copy(destination, source, length)
if filled && copied {
    print(mem.read_u8(destination, 0))
}
```

Test zero-length operations, overlapping ranges, and a destination that ends exactly at capacity.

## 6. Test design

Add explicit tests for each applicable category:

- **Normal input:** the intended operation succeeds.
- **Minimum input:** the smallest valid value behaves according to the contract.
- **Boundary input:** the exact final valid offset, capacity, or length.
- **One past the boundary:** rejection occurs safely.
- **Malformed input:** inconsistent values are rejected.
- **Failure atomicity:** state remains unchanged after rejection.
- **Repeated calls:** the state remains coherent across multiple operations.
- **Integration:** the feature works through the public module boundary.

A test should have a stable name, an explicit expected result, and a failure message identifying the invariant that broke.

## 7. Debugging workflow

1. Reproduce the failure with one fixed input.
2. Record the input and expected result.
3. Find the first point where actual state differs.
4. Inspect both caller and callee contracts.
5. Change one thing only.
6. Recompile and rerun the reproducer.
7. Run all prior regression tests.
8. Keep the reproducer as a permanent test.

Do not test questionable memory by dereferencing it to see what happens. A crash-free execution does not prove that an access was valid.

## 8. Common failure modes

- **Coding before reading the API:** names can hide ownership and failure behavior.
- **Updating state early:** later rejection leaves the module inconsistent.
- **Testing only successful calls:** boundary bugs remain invisible.
- **Mixing policy and parsing:** logic becomes difficult to reuse.
- **Ignoring return values:** an allocation or write may have failed.
- **Using magic offsets:** protocol and memory layouts become hard to audit.
- **Hiding unsupported cases:** callers may assume an incomplete feature is production-ready.
- **Changing many files at once:** the cause of a regression becomes unclear.

## 9. Apply this lesson to the real project

Validate source and destination independently before calling helpers.

Make the change in `examples/courses/memory-lab/`. Keep the implementation in its owning module, the demonstration in the entry point, and regression cases in tests. If the language lacks a feature you expected, adapt the design to supported syntax and record the limitation instead of pretending the syntax exists.

## 10. Exercise

Fill one region, copy into another, inspect positions, and follow the actual zero-length contract.

Extend the exercise with one additional case not covered by the starter. Explain the bug it would catch and the expected outcome before running it.

## 11. Review questions

1. What exact invariant does the operation preserve?
2. Which inputs must be rejected?
3. What is the final possible failure point before state changes?
4. Which API behavior did you verify in source?
5. Which test proves the boundary is correct?
6. How would you reduce a failure to a deterministic reproducer?
7. What does this implementation explicitly not support?

## 12. Completion checklist

- [ ] I inspected the implementation, not just the API name.
- [ ] I can explain every validation check.
- [ ] I added a successful test and boundary/failure tests.
- [ ] A failed operation leaves state unchanged.
- [ ] The entry point calls the module instead of duplicating internals.
- [ ] I ran the relevant compiler and test commands and recorded their real result.
- [ ] I documented unsupported behavior.

## Checkpoint

Which risk does overlap-safe copying solve, and which remains?

Before moving on, write down the invariant you are preserving and the evidence collected. If the lesson disagrees with current source, inspect the implementation rather than guessing. See [the raw-memory contract](../../../raw-memory.md).

Explain the change to another beginner without saying only that it "works". Describe the invariant, validation order, success path, failure path, and the test that catches a regression.

**Why this matters:** Raw pointers do not carry bounds, ownership, or lifetime metadata. Correctness must come from your checks and module contract.

Continue only when you can explain both what the code does and why each state-changing or potentially unsafe operation is valid.
